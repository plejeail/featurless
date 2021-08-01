#include "featurless/log.h"

#if defined(_MSC_VER)
#include <malloc.h>
#elif defined(__GNUC__)
#include <alloca.h>
#endif
#include <atomic>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

featurless::log featurless::log::_instance;

constexpr long SECONDS_PER_DAY = 86400UL;

class FileStream {
    static constexpr int _open_tries = 5;
    static constexpr std::chrono::milliseconds _open_interval{ 10 };
    std::FILE* _fd{ nullptr };

public:
    explicit FileStream() noexcept = default;
    FileStream(const FileStream&) = delete;
    FileStream(FileStream&&) = delete;
    FileStream& operator=(const FileStream&) = delete;
    FileStream& operator=(FileStream&&) = delete;

    ~FileStream() noexcept {
        flush();
        close();
    }

    void open(const std::string_view fname) {
        for (int tries = 0; tries < _open_tries; ++tries) {
#if defined(_WIN32) && !defined(__MINGW32__)
            // Fix MSVC warning about "unsafe" fopen.
            fopen_s(&_fd, fname.data(), "ab");
#else
            _fd = std::fopen(fname.data(), "ab");
#endif
            if (_fd != nullptr) [[likely]] {
                return;
            }
            std::this_thread::sleep_for(_open_interval);
        }
        throw("featurless::log failed to open log file.");
    }

    void flush() { std::fflush(_fd); }

    void close() noexcept {
        if (_fd != nullptr) [[likely]] {
            std::fclose(_fd);
            _fd = nullptr;
        }
    }

    void write(const char* buf, std::size_t bufsize) {
        std::size_t written = std::fwrite(buf, 1, bufsize, _fd);
        if (written != bufsize) [[unlikely]] {
            throw("featurless::log failed writing record to log file.");
        }
    }
};

static time_t midnight_time(time_t t) noexcept {
    // return timestamp of next midnight after t
    return SECONDS_PER_DAY + t - (t % SECONDS_PER_DAY);
}

static time_t get_tz_gm_diff(time_t timer) {
    // return diff between gmtime and local time
    struct tm* lc = localtime(&timer);
    const time_t lct = mktime(lc);
    const time_t dst = (lc->tm_isdst ^ 0) * 3600l;
    return lct - mktime(gmtime(&timer)) + dst;
}

static time_t __tz_diff(time_t timer) noexcept {
    // return diff between gmtime and local time, only once per day
    // after that, return always the same value until next day
    static time_t tz = get_tz_gm_diff(timer);
    static time_t next_check_time = midnight_time(timer);
    if (timer > next_check_time) [[unlikely]] {
        tz = get_tz_gm_diff(timer);
        next_check_time = midnight_time(timer);
    };
    return tz;
}

struct featurless::log::impl {
    impl() noexcept = default;
    ~impl() noexcept = default;
    impl(const impl&) = delete;
    impl(impl&&) = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(impl&&) = delete;

    FileStream _ofstream;
    std::atomic<std::size_t> _current_file_size{ 0 };
    std::size_t _max_file_size{ 0 };
    std::mutex _mutex;
    short _max_files{ 0 };

    std::string _file_path;
    std::string _file_name;
    std::string _file_ext;
};

struct small_tm {
    small_tm() noexcept = default;
    ~small_tm() noexcept = default;
    small_tm(const small_tm&) = delete;
    small_tm(small_tm&&) = delete;
    small_tm& operator=(const small_tm&) = delete;
    small_tm& operator=(small_tm&&) = delete;

    uint16_t tm_sec;   // Seconds. [0-60] (1 leap second)
    uint16_t tm_min;   // Minutes. [0-59]
    uint16_t tm_hour;  // Hours.   [0-23]
    uint16_t tm_mday;  // Day.     [1-31]
    uint16_t tm_mon;   // Month.   [0-11]
    uint16_t tm_year;  // Year     1900.
};

static void parse_time(small_tm& date, time_t timer) noexcept {
    // from arduino glibc, tricky.
    constexpr uint16_t DAYS_PER_CENTURY = 36525U;
    constexpr uint16_t DAYS_PER_4_YEARS = 1461U;
    // Break down timer into whole and fractional parts of 1 day
    uint16_t days = timer / SECONDS_PER_DAY;
    uint16_t fract = static_cast<uint16_t>(timer % SECONDS_PER_DAY);

    // Extract hour, minute, and second from the fractional day
    date.tm_sec = fract % 60;
    fract /= 60;
    date.tm_min = fract % 60;
    date.tm_hour = fract / 60;
    /* Our epoch year has the property of being at the conjunction of
     * all three 'leap cycles', 4, 100, and 400 years ( though we can
     * ignore the 400 year cycle in this library). Using this property,
     * we can easily 'map' the time stamp into the leap cycles, quickly
     * deriving the year and day of year, along with the fact of whether
     * it is a leap year.
    */
    // Map into a 100 year cycle
    uint16_t years = 100 * (days / DAYS_PER_CENTURY);
    uint16_t remaining_days = days % DAYS_PER_CENTURY;

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
    if (days > n) {
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
    if (days < n) {
        /* special case: Jan/Feb month pair */
        date.tm_mon = days / 31;
        date.tm_mday = days % 31;
    } else {
        /* The remaining 10 months form a regular pattern of 31 day months
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
}

static void featurless_localtime_s(small_tm& date) noexcept {
    // custom date parsing
    // do not redo time zone stuff at each time, instead only check one time per
    // day the diff with gmtime.
    // also seems to avoid some system calls / code cache miss
    time_t time_now;  // NOLINT
    time(&time_now);
    parse_time(date, time_now + __tz_diff(time_now));
}

inline std::size_t estimate_record_size(std::size_t dynamic_size) noexcept {
    return 45 + dynamic_size;
}

template<typename int_t>
inline void copy_hex(char* dest, int_t integer) noexcept {
    // starting from the less significant digit and formating backward as
    // hexadecimal until end of integer. (all char digits are assumed to be set
    // to 0 before
    constexpr std::array<char, 16> digits{ '0', '1', '2', '3', '4', '5', '6', '7',
                                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    while (integer > 0) {
        *dest = digits[integer % 16];
        integer /= 16;
        --dest;
    }
}

inline auto fucking_std_thread_id() noexcept {
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

template<typename int_t>
static void copy_time(char* dest, int_t integer) noexcept {
    // copy integers in interval [0, 99]
    // assume to have 2 chars in string reserved
    int_t quot = (103 * integer) >> 10;  // divide by 10
    dest[0] = static_cast<char>('0' + quot);
    dest[1] = static_cast<char>('0' + integer - quot * 10);
}

void featurless::log::write(const char* const __restrict lvl_str,
                            const std::string_view function,
                            const std::string_view message) {
    const std::size_t length_buffer = estimate_record_size(function.size() + message.size());

    small_tm time_info;
    featurless_localtime_s(time_info);
#if defined(_MSC_VER)
    char* msg_buffer = reinterpret_cast<char*>(_malloca(length_buffer));
#elif defined(__GNUC__)
    char* msg_buffer = reinterpret_cast<char*>(alloca(length_buffer));
#else
    char* msg_buffer = reinterpret_cast<char*>(malloc(length_buffer));
#endif
    std::memcpy(msg_buffer, "2000-00-00 00:00:00 [     ][000000000000](", 42);
    constexpr std::size_t offset_write{ 0 };
    // copy date
    copy_time(msg_buffer + 2, time_info.tm_year);
    copy_time(msg_buffer + 5, time_info.tm_mon + 1);
    copy_time(msg_buffer + 8, time_info.tm_mday);
    // copy hour
    copy_time(msg_buffer + 11, time_info.tm_hour);
    copy_time(msg_buffer + 14, time_info.tm_min);
    copy_time(msg_buffer + 17, time_info.tm_sec);
    // level
    std::memcpy(msg_buffer + 21, lvl_str, 5);
    // thread id
    copy_hex(msg_buffer + 39, fucking_std_thread_id());
    // function
    char* ptr_data = msg_buffer + 42;
    std::memcpy(ptr_data, function.data(), function.size());
    ptr_data += function.size();
    *ptr_data++ = ')';
    // record message
    *ptr_data++ = ' ';
    std::memcpy(ptr_data, message.data(), message.size());
    ptr_data[message.size()] = '\n';

    // write log
    _data->_mutex.lock();
    if ((_data->_current_file_size + length_buffer) > _data->_max_file_size && _data->_max_files > 0) [[unlikely]]
        rotate();
    _data->_current_file_size += length_buffer;
    _data->_ofstream.write(msg_buffer, length_buffer);
    _data->_mutex.unlock();
#if !defined(_WIN32) && !defined(__GNUC__)
    free(msg_buffer);
#endif
}

void featurless::log::rotate() {
    _data->_ofstream.close();
    std::string filename_current;
    std::string filename_new;
    for (int file_number = _data->_max_files - 2; file_number >= 0; --file_number) {
        std::error_code nothrow_if_fail;
        build_file_name(filename_current, file_number);
        build_file_name(filename_new, file_number + 1);
        std::filesystem::rename(filename_current, filename_new, nothrow_if_fail);
    }

    _data->_current_file_size = 0;
    _data->_ofstream.open(filename_current);
}

void featurless::log::build_file_name(std::string& filename, int file_number) {
    constexpr std::size_t estimated_number_digits = 2;  // if more than 2 digits, 2x allocation
    filename.reserve(_data->_file_name.size() + _data->_file_ext.size() + estimated_number_digits);
    filename.resize(0);
    filename += _data->_file_name;
    if (file_number > 0) {
        filename += '.';
        filename += std::to_string(file_number);
    }
    if (!_data->_file_ext.empty()) {
        filename += _data->_file_ext;
    }
}

void featurless::log::init(const char* logfile_path, std::size_t max_size_kB, short max_files) {
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

    _instance._data->_ofstream.open(logfile_path);
}

featurless::log::~log() {
    delete _data;
}
