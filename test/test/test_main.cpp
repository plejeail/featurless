#include <featurless/test.h>

bool truefun()
{
    return true;
}

int main(int argc, const char** argv)
{
    featurless::test tester;
    tester.parse_args(argc, argv);

    auto falsefun = []() -> bool
    {
        return false;
    };
    tester.add_group("group0");
    tester.add_group("groupA");
    tester.add_group("groupB");
    tester.add_group("groupC");

    tester.check("mygroupwhichdoesnotexist", "I must not appear my group does not exist.", true);

    tester.check("group0", "success", true);

    tester.check("groupA", "success", true);
    tester.check("groupA", "failure group A", false);
    tester.check("groupA", "success again", truefun);
    tester.require("groupA", "total failure group A", falsefun);
    tester.check("groupA", "success never reached", true);

    tester.check("groupB", "success", true);
    tester.check("groupB", "failure group B", false);
    tester.check("groupB", "success again", truefun);
    tester.require("groupB", "total failure group B", falsefun);
    tester.check("groupB", "success never reached", true);

    tester.check("success", true);
    tester.check("failure global", false);
    tester.check("success again", truefun);
    tester.require("total failure global", falsefun);
    tester.check("success never reached", true);

    tester.check("groupC", "success", true);
    tester.check("groupC", "failure group C", false);
    tester.check("groupC", "success again", truefun);
    tester.require("groupC", "total failure group C", falsefun);
    tester.check("groupC", "success never reached", true);

    tester.print_summary();
    return tester.status();
}

// Expected:
// Total 22 checks
// - check unkown group 0/0
// group 0  (1/1)
//  - check success 1/1     1/1
// group A  (2/4)
//  - check success 1/1     2/2
//  - check failure 1/2     2/3
//  - check success 2/3     3/4
//  - require failure 2/4   3/5
//  - check success 2/4     3/5
// group B  (2/4)
//  - check success 1/1     4/6
//  - check failure 1/2     4/7
//  - check success 2/3     5/8
//  - require failure 2/4   5/9
//  - check success 2/4     5/9
// global   (6/12)
//  - check success NA      6/10
//  - check failure NA      6/11
//  - check success NA      7/12
//  - require failure NA    7/13
//  - check success NA      7/13
// group C (0/0)
//  - check success NA      7/13
//  - check failure NA      7/13
//  - check success NA      7/13
//  - require failure NA    7/13
//  - check success NA      7/13
