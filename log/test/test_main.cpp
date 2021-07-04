#include <chrono>
#include <iostream>

#define FEATURLESS_LOG_MIN_LEVEL FLOG_LEVEL_TRACE
#define FEATURLESS_LOG_USE_UTC
#include <featurless/log.h>

int main()
{
    std::iostream::sync_with_stdio(false);

    featurless::log::init("./Logs/myapp.log", 400, 5);
    FLOG_DEBUG("OK TEST");
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < 3; ++i)
    {
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
        FLOG_DEBUG("OK TEST");
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

    for (int i = 0; i < 3; ++i)
    {
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
        std::cout
          << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
    }
    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
    std::cout
      << "time (ns): "
      << std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count() / 30 << '\n'
      << "base comparison: "
      << std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1) - (t3 - t2)).count() / 30
      << "ns ("
      << static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count())
           / static_cast<float>(
             std::chrono::duration_cast<std::chrono::nanoseconds>((t3 - t2)).count())
      << ")\n";
}
