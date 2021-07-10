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
//      FLOG_TRACE("THIS MESSAGE IS NOT LOGGED");
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
#define FLOG_TRACE(message)                                                                \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::trace>(),                      \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_TRACE(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_DEBUG
#define FLOG_DEBUG(message)                                                                \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::debug>(),                      \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_DEBUG(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_INFO
#define FLOG_INFO(message)                                                                 \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::info>(),                       \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_INFO(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_WARN
#define FLOG_WARN(message)                                                                 \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::warning>(),                    \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_WARN(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_ERROR
#define FLOG_ERROR(message)                                                                \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::error>(),                      \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_ERROR(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_FATAL
#define FLOG_FATAL(message)                                                                \
    featurless::log::logger().write<FEATURLESS_LOG_TIME_UTC>(                              \
      featurless::__level_to_string<featurless::log::level::fatal>(),                      \
      __FEATURLESS_STRINGIZE(__LINE__), __func__, featurless::__pretty_filename(__FILE__), \
      message)
#else
#define FLOG_FATAL(message)
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

    static void init(const char* logfile_path, size_t max_size_kB = 0, short max_files = 0);

    static log& logger() noexcept { return _instance; }

    template<bool use_utc>
    void write(const std::string_view lvl_str,
               const std::string_view line,
               const std::string_view function,
               const std::string_view src_file,
               const std::string_view message);

    ~log();

private:
    static log _instance;

    template<bool use_utc>
    void write_record(const std::string_view lvl_str,
                      const std::string_view line,
                      const std::string_view function,
                      const std::string_view src_file,
                      const std::string_view message);

    std::string build_file_name(int file_number = 0);

    void rotate();

    struct impl;
    impl* _data;
};

#if FEATURLESS_LOG_MIN_LEVEL < FLOG_LEVEL_NONE
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
#endif
}  // namespace featurless
#endif  // FEATURLESS_LOG_HEADER_GUARD

