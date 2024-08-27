#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, label_arg)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ jump(x); l̲a̲b̲e̲l̲ x;
                g̲o̲t̲o̲ x;

            print(123);
            jump(L);
            print(456);
    L:      print(789);
        e̲n̲d̲
    )");
    const std::string expect = R"(123
789
)";
    EXPECT_EQ(output, expect);
}

//TODO: need to implement SSI
TEST_F(x1_machine, DISABLED_switch_one_two_three)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ i;
            s̲w̲i̲t̲c̲h̲ sw := one, two, three, done;
            i := 0;
    next:   i := i + 1
            g̲o̲ t̲o̲ sw[i];
    three:  print(33);
            g̲o̲ t̲o̲ next;
    two:    print(22);
            g̲o̲ t̲o̲ next;
    one:    print(11);
            g̲o̲ t̲o̲ next;
    done:   print(0);
        e̲n̲d̲
    )");
    const std::string expect = R"(11
22
33
0
)";
    EXPECT_EQ(output, expect);
}
