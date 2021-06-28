# Featurless Test, a simple C++ test framework
This library provide a simple testing class. Ease-of-use and fast-compile time are the main concerns of the library. Do not forget that if you find a feature, it's a bug.
Please report it, correct it, or do whatever you want with it.

Tolerated features (considered as *only minor* bugs) are:
- parsing arguments
- displaying a program help
- grouping tests
- enabling only some groups of tests at runtime
- disabling only some tests at runtime
- performing breaking tests (that will stop testing) at a group/global level

You can't disable and enable groups at the same time. It would be a powerful (hence useless) feature.
By the way, arguments will only work if you parse arguments.

## Library usage
First, you must provide your own main function, include the library and declare a test object:
```
#include <featurless/test.h>
int main(int argc, const char** argv)
{
    featurless::test tester;
}
```
The test object can parse arguments (display help/enable groups/disable groups) by using the following method:
```
    tester.parse_args(argc, argv);
```

To add a group use the **add_group(group_name)** method:
```
    tester.add_group("mygroupname");
```

To add a *check*, that will not end the test of the group use the **check(group_name, description, value)** method:
```
 tester.check("mygroupname", "this is what my test do", true);
```
If the group does not exist, the check will be ignored and a warning will be printed at runtime.
You can also provide a function(lambda if decaying to function) that takes no arguments and return a boolean:
```
 tester.check("mygroupname", "evaluate a function of 'type' bool f(void)", []() { return false; });
```

To add a *require*, that will end testing a group, use the **require(group_name, description, value)** method:
```
 tester.require("group0", "this would stop if it was false", true);
```
A 'group require' only stop testing for **its** group.
Of course, functions are also supported by require:
```
    bool myfun() { return true; }
    tester.check("mygroupname", "also supporting functions", myfun);
```

You can make a global check or require by using any of the method presented above without using the **group_name** argument. Therefore, the method args will be (description, value). You can alse use functions with them.
```
    tester.check("Evaluating a global test", []() { return false; });
```
Global tests can not be disabled. They are always evaluated (if no global require failed already). If a global require fails, it will end the evaluation of all the test following it.

At the end of your test program you can print a summary of your test in stdout using the following:
```
    tester.print_summary();
```

The tester instance can return an error code (0: Ok, 1: Argument parsing error, 2: require test failed).
```
    tester.status();
```

For more informations, look at test/test_main.cpp or at the source code.

## Generated program arguments
Arguments:
    -h, --help        display this help and exit
    -e, --enable      enable only the groups of tests whose name is provided after this argument.
    -d, --disable     disable only the groups of tests whose name is provided after this argument.

You can do that: program -e group1 group3 group4
You can do that: program -d group2
You can NOT do that: program -e group1 group3 group4 -d group2
