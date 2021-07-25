#include <chrono>
#include <cstring>
#include <fmt/format.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#define FEATURLESS_LOG_MIN_LEVEL FLOG_LEVEL_TRACE
//#define FEATURLESS_LOG_USE_UTC
#include <featurless/log.h>


void logging()
{
    for (int i = 0; i < 2500; ++i)
    {
        FLOG_DEBUG("As a data scientist");
        FLOG_DEBUG(fmt::format("{} {} {} {}", "As", "a", 12.3453, "scientist"));
    }
}


// copy all of that inside its own header, namespace
// estimate max args size => give max formated string size
// then format directly inside the buffer

template<typename T>
void print(std::string name, T value)
{
    char buffer[64];
    // char* buffer_end = format_arg<T>(buffer, value);
    // *buffer_end = '\0';
    std::cout << name << " format: |" << buffer << "| (" << value << ")\n";
}
// storing all args constexpr ? bof
// format function constexpr

int main()
{
    // int a = 1;
    // short b = 2;
    // long long c = 5;
    // int* d = nullptr;
    // unsigned int e = 1;
    // float f = 2.f;
    // double g = 2.0;
    // long double h = 2.0;
    // char i = 'a';
    // char j[]{ "this is it" };
    // std::string k = "this is it";
    // std::cout << "int: " << inner_estimate(a) << '\n';
    // std::cout << "short: " << inner_estimate(b) << '\n';
    // std::cout << "long: " << inner_estimate(c) << '\n';
    // std::cout << "pointer: " << inner_estimate(d) << '\n';
    // std::cout << "unsigned: " << inner_estimate(e) << '\n';
    // std::cout << "float: " << inner_estimate(f) << '\n';
    // std::cout << "double*: " << inner_estimate(g) << '\n';
    // std::cout << "unsigned: " << inner_estimate(h) << '\n';
    // std::cout << "char: " << inner_estimate(i) << '\n';
    // std::cout << "char**: " << inner_estimate(j) << '\n';
    // std::cout << "string*: " << inner_estimate(k) << '\n';
    // std::to_string(0.0f);
    // std::cout << "## signed integers: \n";
    // print<char>("char", 'a');
    // print<short>("short", 12);
    // print<int>("int", 253943);
    // print<long>("long", 340394);
    // print<long long>("long long", 3037583649);
    // std::cout << "## unsigned integers: \n";
    // print<bool>("bool", true);
    // print<bool>("bool", false);
    // print<unsigned char>("unsigned char", 'Z');
    // print<unsigned short>("unsigned short", 98);
    // print<unsigned int>("unsigned int", 305389);
    // print<unsigned long>("unsigned long", 140192);
    // print<unsigned long long>("unsigned long long", 18792);
    // std::cout << "## floating point types: \n";
    // print<float>("float", 0.15);
    // print<double>("double", 25.245849258);
    // print<long double>("long double", 39853.29358);
    // std::cout << "## string types: \n";
    // char mescouilles[]{ "test toi" };
    // print<char*>("cstring", mescouilles);
    // print<cxx_string>("cxx_string", "test moi");
    // void* larnaque = reinterpret_cast<void*>(mescouilles);
    // print<void*>("void*", larnaque);

    std::iostream::sync_with_stdio(false);
    featurless::log::init("myapp.log", 2000, 10, 512);
    FLOG_DEBUG("OK TEST");
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::thread a(logging);
    std::thread b(logging);
    std::thread c(logging);
    std::thread d(logging);
    std::thread e(logging);
    std::thread f(logging);
    std::thread g(logging);
    std::thread h(logging);
    a.join();
    b.join();
    c.join();
    d.join();
    e.join();
    f.join();
    g.join();
    h.join();
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    // for (int i = 0; i < 3000; ++i)
    // {
    //     std::cout << "[2021-06-28 11:41:52][000000000000][debug][main]@(test_main.cpp,20) OK TEST\n";
    // }
    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();

    std::stringstream s;
    s << std::hex << std::this_thread::get_id() << '\n';
    std::cout << "thread: " << s.str();

    std::cout << "total(mus): " << std::chrono::duration_cast<std::chrono::microseconds>((t2 - t1)).count() << '\n'
              << "avg time (ns): " << std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count() / 40000
              << '\n'
              << "avg base (ns): " << std::chrono::duration_cast<std::chrono::nanoseconds>((t3 - t2)).count() / 3000
              << " ("
              << static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>((t2 - t1)).count())
                   / static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>((t3 - t2)).count())
              << ")\n";
}