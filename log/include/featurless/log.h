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

#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_TRACE
#define FLOG_TRACE(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::trace>(), __func__, message)
#else
#define FLOG_TRACE(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_DEBUG
#define FLOG_DEBUG(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::debug>(), __func__, message)
#else
#define FLOG_DEBUG(message)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_INFO
#define FLOG_INFO(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::info>(), __func__, message)
#else
#define FLOG_INFO(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_WARN
#define FLOG_WARN(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::warning>(), __func__, message)
#else
#define FLOG_WARN(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_ERROR
#define FLOG_ERROR(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::error>(), __func__, message)
#else
#define FLOG_ERROR(...)
#endif
#if FEATURLESS_LOG_MIN_LEVEL <= FEATURLESS_LOG_LEVEL_FATAL
#define FLOG_FATAL(message) \
    featurless::log::logger().write(featurless::__level_to_string<featurless::log::level::fatal>(), __func__, message)
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

    inline void write(const char* const lvl_str, const std::string_view function, const std::string_view message)
    {
        write_record(lvl_str, function, message);
    }
    ~log();

private:
    log() = default;
    log(log&) = delete;
    log(log&&) = delete;

    static log _instance;

    void write_record(const char* const lvl_str, const std::string_view function, const std::string_view message);

    void build_file_name(std::string& filename, int file_number);

    void rotate();

    struct impl;
    impl* _data{ nullptr };
};

#if FEATURLESS_LOG_MIN_LEVEL < FEATURLESS_LOG_LEVEL_NONE
template<featurless::log::level lvl>
consteval const char* const __level_to_string() noexcept
{
    using level = featurless::log::level;
    if constexpr (lvl == level::trace)
        return "trace";
    else if constexpr (lvl == level::debug)
        return "debug";
    else if constexpr (lvl == level::info)
        return "info ";
    else if constexpr (lvl == level::warning)
        return "warn ";
    else if constexpr (lvl == level::error)
        return "error";
    else if constexpr (lvl == level::fatal)
        return "fatal";
    else
        return " ??? ";
}
// #include "format.h"
#endif
}  // namespace featurless
#endif  // FEATURLESS_LOG_HEADER_GUARD
