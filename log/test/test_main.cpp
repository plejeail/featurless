#include <chrono>
#include <iostream>

#define FEATURLESS_LOG_MIN_LEVEL FLOG_LEVEL_TRACE
#include <featurless/log.h>

int main()
{
    std::iostream::sync_with_stdio(false);

    featurless::log::init("Logs/myapp.log", 4, 5);
    FLOG_DEBUG("OK TEST");
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
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
    FLOG_DEBUG("OK TEST");
    FLOG_DEBUG("OK TEST");
    FLOG_DEBUG("OK TEST");
    FLOG_DEBUG("OK TEST");
    FLOG_DEBUG("OK TEST");
    FLOG_DEBUG("OK TEST");
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::cout << "[2021-06-28][11:41:52][00000000][debug][test_main.cpp,20]@(main) > OK TEST\n";
    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
    std::cout << "time (ns): "
              << std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1) - (t3 - t2)).count()
                   / 10
              << '\n';
}
// clang-format off
// include tests headers
// clang-format on
