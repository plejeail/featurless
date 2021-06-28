//===-- test.h ------------------------------------------------------------===//
//                          SIMPLE TESTS LIBRARY
//
// Provide simple testing class. Ease-of-use and fast-compile time are
// the main concerns of the library. If you find a feature in it, it's a bug.
// Please report it, correct it, or do whatever you want with it.
//
// Tolerated features (considered as minor bugs) are:
// - parsing arguments
// - displaying a program help printed with -h or --help arguments.
// - grouping tests
// - enabling only some groups of tests
// - disabling only some tests.
// You can't disable and enable at the same time. It would be a uselessely
// powerful feature.
//
// Enabling and disabling, will only work if you parse args.
//
// Code:
//     int main(int argc, char** argv)
//     {
//       featurless::test tester;       // you must define it yourself
//
//       tester.parse_args(argc, args);         // if you want to do it, you can
//
//       tester.group("group_name");            // its a feature hence a bug
//       tester.assert("group_name", "If I fail, this group tests stop here, not all tests");
//       tester.check("group_name",  "I am testing if I am the pope", 1 == 2);
//       tester.summary("group_name");    // print testing results for group "group_name"
//
//       tester.check("I dont need a group for my test/assert.", true);
//
//       // a function I want to execute as a test (type: bool(*func)(void))
//       auto func_to_test = []() -> bool { return false; }
//       tester.check("another bug...", func_to_test);
//       tester.return();
//  }
// Generated program arguments:
// Execute the featurless tests you just compiled.
// Arguments:
//    -h, --help        display this help and exit
//    -e, --enable      enable only the groups of tests whose name is provided
//                      after this argument.
//    -d, --disable     disable only the groups of tests whose name is provided
//                      after this argument.
//
// You can do that: program -e group1 group3 group4
// You can do that: program -d group2
// you can NOT do that: program -e group1 group3 group4 -d group2
//
//===----------------------------------------------------------------------===//
#ifndef FEATURLESS_TESTS_HEADER_GUARD
#define FEATURLESS_TESTS_HEADER_GUARD

namespace featurless
{
class test
{
    struct Data;
    typedef bool testfun_t();
    static constexpr int line_width{ 73 };

public:
    test();
    ~test();
    test(test&) = delete;
    test& operator=(test&) = delete;
    test(test&&) = delete;

    void parse_args(int argc, const char** argv);

    void add_group(const char* const group_name);

    void require(const char* const description, bool condition);
    void require(const char* const description, testfun_t condition);
    void require(const char* const group_name, const char* const description, bool condition);
    void require(const char* const group_name, const char* const description, testfun_t condition);

    void check(const char* const description, bool condition);
    void check(const char* const description, testfun_t condition);
    void check(const char* const group_name, const char* const description, bool condition);
    void check(const char* const group_name, const char* const description, testfun_t condition);

    void print_summary() const;

    int status() const noexcept;

private:
    bool is_group_enabled(const char* const group_name);
    bool internal_check(const char* const description, bool condition);
    bool
    internal_check(const char* const description, const char* const group_name, bool condition);
    Data* _data;
};
}  // namespace featurless
#endif  // FEATURLESS_TESTS_HEADER_GUARD
