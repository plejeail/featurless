#include "featurless/log.h"
#include <alloca.h>
#include <atomic>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

#include <iostream>
featurless::log featurless::log::_instance;

constexpr long SECONDS_PER_DAY = 86400UL;

static time_t midnight_time(time_t t) noexcept
{
    // return timestamp of next midnight after t
    return 86400UL + t - (t % SECONDS_PER_DAY);
}

static time_t get_tz_gm_diff(time_t timer)
{
    // return diff between gmtime and local time
    struct tm* lc = localtime(&timer);
    const time_t lct = mktime(lc);
    const time_t dst = (lc->tm_isdst > 0) ? 3600 : 0;
    return lct - mktime(gmtime(&timer)) + dst;
}

static time_t __tz_diff(time_t timer) noexcept
{
    // return diff between gmtime and local time, only once per day
    // after that, return always the same value until next day
    static time_t tz = get_tz_gm_diff(timer);
    static time_t next_check_time = midnight_time(timer);
    if (timer > next_check_time) [[unlikely]]
    {
        tz = get_tz_gm_diff(timer);
        next_check_time = midnight_time(timer);
    };
    return tz;
}

struct featurless::log::impl
{
    char* _streambuffer{ nullptr };
    std::ofstream _ofstream;
    std::atomic<std::size_t> _current_file_size{ 0 };
    std::size_t _max_file_size{ 0 };
    short _max_files{ 0 };

    std::string _file_path;
    std::string _file_name;
    std::string _file_ext;
    std::mutex _mutex;
};

struct small_tm
{
    int tm_sec;   // Seconds. [0-60] (1 leap second)
    int tm_min;   // Minutes. [0-59]
    int tm_hour;  // Hours.	  [0-23]
    int tm_mday;  // Day.     [1-31]
    int tm_mon;   // Month.   [0-11]
    int tm_year;  // Year     1900.
};

static small_tm parse_time(time_t timer) noexcept
{
    // from arduino glibc, tricky.
    small_tm date;
    constexpr long DAYS_PER_CENTURY = 36525L;
    constexpr long DAYS_PER_4_YEARS = 1461L;
    // Break down timer into whole and fractional parts of 1 day
    uint16_t days = timer / SECONDS_PER_DAY;
    long fract = timer % SECONDS_PER_DAY;

    // Extract hour, minute, and second from the fractional day
    date.tm_sec = fract % 60L;
    fract /= 60L;
    date.tm_min = fract % 60L;
    date.tm_hour = fract / 60L;
    /* Our epoch year has the property of being at the conjunction of
     * all three 'leap cycles', 4, 100, and 400 years ( though we can
     * ignore the 400 year cycle in this library). Using this property,
     * we can easily 'map' the time stamp into the leap cycles, quickly
     * deriving the year and day of year, along with the fact of whether
     * it is a leap year.
    */
    // Map into a 100 year cycle
    uint16_t years = 100 * ((long)days / DAYS_PER_CENTURY);
    long remaining_days = (long)days % DAYS_PER_CENTURY;

    // Map into a 4 year cycle
    years += 4 * (remaining_days / DAYS_PER_4_YEARS);
    days = remaining_days % DAYS_PER_4_YEARS;
    if (years > 100)
        days++;

    /*
     * 'years' is now at the first year of a 4 year leap cycle, which
     * will always be a leap year, unless it is 100. 'days' is now an
     * index into that cycle.
    */
    uint16_t leapyear = 1;
    if (years == 100)
        leapyear = 0;

    // Compute length, in days, of first year of this cycle
    uint16_t n = 364 + leapyear;

    /*
     * If the number of days remaining is greater than the length of the
     * first year, we make one more division.
     */
    if (days > n)
    {
        days -= leapyear;
        leapyear = 0;
        years += days / 365;
        days %= 365;
    }
    date.tm_year = years - 30;
    /*
     * Given the year, day of year, and leap year indicator, we can
     * break down the month and day of month. If the day of year is less
     * than 59 (or 60 if a leap year), then we handle the Jan/Feb month
     * pair as an exception.
     */
    n = 59 + leapyear;
    if (days < n)
    {
        /* special case: Jan/Feb month pair */
        date.tm_mon = days / 31;
        date.tm_mday = days % 31;
    }
    else
    {
        /*
       * The remaining 10 months form a regular pattern of 31 day months
       * alternating with 30 day months, with a 'phase change' between
       * July and August (153 days after March 1). We proceed by mapping
       * our position into either March-July or August-December.
       */
        days -= n;
        auto temp = date.tm_mon = 2 + (days / 153) * 5;

        // Map into a 61 day pair of months
        days %= 153;
        date.tm_mon += (days / 61) * 2;

        // Map into a month/day
        days %= 61;
        date.tm_mon += days / 31;
        date.tm_mday = days % 31;
    }
    ++date.tm_mday;
    return (date);
}

static small_tm featurless_localtime_s() noexcept
{
    // custom date parsing
    // do not redo time zone stuff at each time, instead only check one time per
    // day the diff with gmtime.
    // also seems to avoid some system calls / code cache miss
    time_t time_now;  // NOLINT
    time(&time_now);
    return parse_time(time_now + __tz_diff(time_now));
}

inline std::size_t estimate_record_size(std::size_t dynamic_size) noexcept
{
    return 50 + dynamic_size;
}

template<typename int_t>
inline void copy_hex(char* dest, int_t integer) noexcept
{
    // starting from the less significant digit and formating backward as
    // hexadecimal until end of integer. (all char digits are assumed to be set
    // to 0 before
    constexpr std::array<char, 16> digits{ '0', '1', '2', '3', '4', '5', '6', '7',
                                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    while (integer > 0)
    {
        *dest = digits[integer % 16];
        integer /= 16;
        --dest;
    }
}

inline auto fucking_std_thread_id() noexcept
{
    // why would any one want to make some fucking thread ids arithmetic ???!!
    // forced to do this in order to avoid slow string streams (>10x slower)
#if _MSC_VER > 0
    auto stupid_std_id = std::this_thread::get_id();
    return *reinterpret_cast<unsigned int*>(&stupid_std_id);
#elif defined(__GNUC__)  // dont know if it works on intel comiler
    auto stupid_std_id = std::this_thread::get_id();
    return *reinterpret_cast<unsigned long int*>(&stupid_std_id);
#else                    // no thread id for you, sorry
    constexpr int noid{ 0 };
    return noid;
#endif
}

static void copy_int(char* dest, int integer) noexcept
{
    // copy integers in interval [0, 99]
    // assume to have 2 chars in string reserved
    dest[0] = static_cast<char>('0' + integer / 10);
    dest[1] = static_cast<char>('0' + integer % 10);
}

void featurless::log::write_record(const std::string_view lvl_str,
                                   const std::string_view line,
                                   const std::string_view function,
                                   const std::string_view src_file,
                                   const std::string_view message)
{
    const std::size_t length_buffer =
      estimate_record_size(line.size() + function.size() + src_file.size() + message.size());
    if ((_data->_current_file_size + length_buffer) > _data->_max_file_size && _data->_max_files > 0) [[unlikely]]
        rotate();

    small_tm time_info = featurless_localtime_s();

#if defined(_WIN32)
    char* msg_buffer = reinterpret_cast<char*>(_alloca(length_buffer));
#elif defined(__GNUC__)
    char* msg_buffer = reinterpret_cast<char*>(alloca(length_buffer));
#else
    char* msg_buffer = reinterpret_cast<char*>(malloc(length_buffer));
#endif
    std::memcpy(msg_buffer, "[2000-00-00 00:00:00][000000000000][     ][", 44);

    copy_int(msg_buffer + 3, time_info.tm_year);
    copy_int(msg_buffer + 6, time_info.tm_mon + 1);
    copy_int(msg_buffer + 9, time_info.tm_mday);
    copy_int(msg_buffer + 12, time_info.tm_hour);
    copy_int(msg_buffer + 15, time_info.tm_min);
    copy_int(msg_buffer + 18, time_info.tm_sec);
    copy_hex(msg_buffer + 33, fucking_std_thread_id());
    std::memcpy(msg_buffer + 36, lvl_str.data(), lvl_str.size());
    char* ptr_data = msg_buffer + 43;
    std::memcpy(ptr_data, function.data(), function.size());
    ptr_data += function.size();
    *(ptr_data++) = ']';
    *(ptr_data++) = '@';
    *(ptr_data++) = '(';
    std::memcpy(ptr_data, src_file.data(), src_file.size());
    ptr_data += src_file.size();
    *(ptr_data++) = ',';
    std::memcpy(ptr_data, line.data(), line.size());
    ptr_data += line.size();
    *(ptr_data++) = ')';
    *(ptr_data++) = ' ';
    std::memcpy(ptr_data, message.data(), message.size());
    ptr_data[message.size()] = '\n';

    std::scoped_lock s{ _data->_mutex };
    _data->_current_file_size += length_buffer;
    _data->_ofstream.write(msg_buffer, static_cast<std::streamsize>(length_buffer));

#if !defined(_WIN32) && !defined(__GNUC__)
    free(msg_buffer);
#endif
}

void featurless::log::rotate()
{
    std::scoped_lock s{ _data->_mutex };
    _data->_ofstream.close();

    std::string filename_current;
    std::string filename_new;
    for (int file_number = _data->_max_files - 2; file_number >= 0; --file_number)
    {
        std::error_code nothrow_if_fail;
        build_file_name(filename_current, file_number);
        build_file_name(filename_new, file_number + 1);
        std::filesystem::rename(filename_current, filename_new, nothrow_if_fail);
    }
    _data->_current_file_size = 0;

    _data->_ofstream.open(filename_current, std::ios::binary);
}

void featurless::log::build_file_name(std::string& filename, int file_number)
{
    constexpr std::size_t estimated_number_digits = 2;  // if more than 2 digits, 2x allocation
    filename.reserve(_data->_file_name.size() + _data->_file_ext.size() + estimated_number_digits);
    filename.resize(0);
    filename += _data->_file_name;
    if (file_number > 0)
    {
        filename += '.';
        filename += std::to_string(file_number);
    }
    if (!_data->_file_ext.empty())
    {
        filename += _data->_file_ext;
    }
}


void featurless::log::init(const char* logfile_path,
                           std::size_t max_size_kB,
                           short max_files,
                           std::size_t buffer_size_kB)
{
    _instance._data = new impl();
    if (max_files < 0)
        throw "logger::init max number of files is less than 0";

    _instance._data->_max_file_size = max_size_kB * 1000;
    _instance._data->_max_files = max_files;

    std::filesystem::path p{ logfile_path };
    std::error_code file_size_fail;
    _instance._data->_current_file_size = std::filesystem::file_size(logfile_path, file_size_fail);

    if (file_size_fail)
        _instance._data->_current_file_size = 0;

    _instance._data->_file_ext = p.extension();
    p.replace_extension();
    _instance._data->_file_name = p;
    p.remove_filename();
    if (!p.empty())
        std::filesystem::create_directories(p);

    if (buffer_size_kB > 0)
    {
        buffer_size_kB *= 1000;
        _instance._data->_streambuffer = new char[buffer_size_kB];
        _instance._data->_ofstream.rdbuf()->pubsetbuf(_instance._data->_streambuffer,
                                                      static_cast<std::streamsize>(buffer_size_kB));
    }

    _instance._data->_ofstream.open(logfile_path, std::ios_base::app | std::ios::binary);
}

featurless::log::~log()
{
    _instance._data->_ofstream.flush();
    if (_data != nullptr)
        delete[] _data->_streambuffer;
    delete _data;
}
