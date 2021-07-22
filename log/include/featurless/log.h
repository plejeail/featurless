//===-- logger.h ----------------------------------------------------------===//
//                         SIMPLE LOGGING LIBRARY
//
//
// Featurless small C++ logging library.
//
// It provides the following tolerated features:
// - create folders of log path file if they do not exit
// - several log levels that can be enabled or disabled at compile time
// - rolling file
//
// Usage:
// #define FEATURLESS_LOG_MIN_LEVEL FLOG_LEVEL_DEBUG
// #include<featurless/log.h>
// int main()
// {
//      size_t max_size_kB = 1000;
//      short max_nb_files = 10;
//      featurless::log::init("./my-log-path.log", max_size_kB, max_nb_files);
//      FLOG_TRACE("THIS MESSAGE IS NOT LOGGED because of FEATURLESS_LOG_MIN_LEVEL");
//      FLOG_DEBUG("Starting application");
//      FLOG_FATAL("Abort! Abort! Abort!");
// }
//
// The logger is a singleton that can be accessed with the logger() method.
// However, this should not be necessary.
//
//===----------------------------------------------------------------------===//
#ifndef FEATURLESS_LOG_HEADER_GUARD
#define FEATURLESS_LOG_HEADER_GUARD

#include <string_view>

#define FEATURLESS_LOG_LEVEL_TRACE 0
#define FEATURLESS_LOG_LEVEL_DEBUG 1
#define FEATURLESS_LOG_LEVEL_INFO  2
#define FEATURLESS_LOG_LEVEL_WARN  3
#define FEATURLESS_LOG_LEVEL_ERROR 4
#define FEATURLESS_LOG_LEVEL_FATAL 5
#define FEATURLESS_LOG_LEVEL_NONE  6

#if !defined(FEATURLESS_LOG_MIN_LEVEL)
#define FEATURLESS_LOG_MIN_LEVEL FEATURLESS_LOG_LEVEL_TRACE
#endif

#if defined(FEATURLESS_LOG_USE_UTC)
#define FEATURLESS_LOG_TIME_UTC true
#else
#define FEATURLESS_LOG_TIME_UTC false
#endif

#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_TRACE
#define FLOG_TRACE(...)                                                                                           \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                     \
      featurless::__level_to_string<featurless::log::level::trace>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_TRACE(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_DEBUG
#define FLOG_DEBUG(...)                                                                                           \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                     \
      featurless::__level_to_string<featurless::log::level::debug>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_DEBUG(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_INFO
#define FLOG_INFO(...)                                                                                           \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                    \
      featurless::__level_to_string<featurless::log::level::info>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_INFO(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_WARN
#define FLOG_WARN(...)                                                                                              \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                       \
      featurless::__level_to_string<featurless::log::level::warning>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_WARN(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_ERROR
#define FLOG_ERROR(...)                                                                                           \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                     \
      featurless::__level_to_string<featurless::log::level::error>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_ERROR(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_FATAL
#define FLOG_FATAL(...)                                                                                           \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                                                     \
      featurless::__level_to_string<featurless::log::level::fatal>(), __FEATURLESS_STRINGIZE(__LINE__), __func__, \
      featurless::__pretty_filename(__FILE__), __VA_ARGS__)
#else
#define FLOG_FATAL(...)
#endif

namespace featurless
{
class log
{
public:
    enum class level : char
    {
        trace = 0,
        debug = 1,
        info = 2,
        warning = 3,
        error = 4,
        fatal = 5,
        _nb_levels
    };

    static void
    init(const char* logfile_path, std::size_t max_size_kB, short max_files, std::size_t buffer_size_kB = 256);

    static log& logger() noexcept { return _instance; }

    template<bool use_utc, typename... args>
    inline void write(const std::string_view lvl_str,
                      const std::string_view line,
                      const std::string_view function,
                      const std::string_view src_file,
                      const char* message,
                      args... format_args)
    {
        if constexpr (sizeof...(format_args) > 0)
        {
            char formatted_msg[512];
            snprintf(formatted_msg, 512, message, format_args...);
            write_record<use_utc>(lvl_str, line, function, src_file, formatted_msg);
        }
        else
        {
            write_record<use_utc>(lvl_str, line, function, src_file, message);
        }
    }
    ~log();

private:
    log() = default;
    log(log&) = delete;
    log(log&&) = delete;

    static log _instance;

    template<bool use_utc>
    void write_record(const std::string_view lvl_str,
                      const std::string_view line,
                      const std::string_view function,
                      const std::string_view src_file,
                      const std::string_view message);

    void build_file_name(std::string& filename, int file_number);

    void rotate();

    struct impl;
    impl* _data{ nullptr };
};

#if FEATURLESS_LOG_MIN_LEVEL < FEATURLESS_LOG_LEVEL_NONE
#define __FEATURLESS_STRINGIZE(x)    __FEATURLESS_STRINGIZE_DT(x)
#define __FEATURLESS_STRINGIZE_DT(x) #x

consteval std::string_view __pretty_filename(const std::string_view filename) noexcept
{  // to be used only on __FILE__ macro
    std::string_view::const_reverse_iterator it;
    for (it = filename.rbegin(); it < filename.rend(); ++it)
#ifdef _WIN32
        if (*it == '\\')
            break;
#else
        if (*it == '/')
            break;
#endif
    return std::string_view(it.base(), filename.end());
}

template<featurless::log::level lvl>
consteval std::string_view __level_to_string() noexcept
{
    using level = featurless::log::level;
    if constexpr (lvl == level::trace)
        return std::string_view("trace");
    else if constexpr (lvl == level::debug)
        return std::string_view("debug");
    else if constexpr (lvl == level::info)
        return std::string_view("info ");
    else if constexpr (lvl == level::warning)
        return std::string_view("warn ");
    else if constexpr (lvl == level::error)
        return std::string_view("error");
    else if constexpr (lvl == level::fatal)
        return std::string_view("fatal");
    else
        return std::string_view(" ??? ");
}
// #include "format.h"
#endif
}  // namespace featurless
#endif  // FEATURLESS_LOG_HEADER_GUARD
