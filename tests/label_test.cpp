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

TEST_F(x1_machine, switch_one_two_three)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ i;
            s̲w̲i̲t̲c̲h̲ sw := one, two, three, done;
            i := 0;
    next:   i := i + 1;
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

TEST_F(x1_machine, jump_one_two_three)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ i;
            s̲w̲i̲t̲c̲h̲ sw := one, two, three, done;

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ jump(s, n); s̲w̲i̲t̲c̲h̲ s; i̲n̲t̲e̲g̲e̲r̲ n;
                g̲o̲t̲o̲ s[n];

            i := 0;
    next:   i := i + 1;
            jump(sw, i);
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

TEST_F(x1_machine, switch_xyzt)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            b̲o̲o̲l̲e̲a̲n̲ b;
            i̲n̲t̲e̲g̲e̲r̲ i;
            s̲w̲i̲t̲c̲h̲ s := x, y, i̲f̲ b t̲h̲e̲n̲ z e̲l̲s̲e̲ t;

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ p(s); s̲w̲i̲t̲c̲h̲ s;
                g̲o̲t̲o̲ s[i];

            b := t̲r̲u̲e̲;
            i:= 1;
            p(s);
        t:
            PRINTTEXT(`t'); NLCR;
            g̲o̲t̲o̲ e;
        y:
            i := 3;
            PRINTTEXT(`y');
            p(s);
        z:
            b := f̲a̲l̲s̲e̲;
            PRINTTEXT(`z');
            p(s);
        x:
            i := 2;
            PRINTTEXT(`x');
            p(s);
        e:
        e̲n̲d̲
    )");
    const std::string expect = "xyzt\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, switch_at_level1)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ i;

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ jump(s, n); s̲w̲i̲t̲c̲h̲ s; i̲n̲t̲e̲g̲e̲r̲ n;
                g̲o̲t̲o̲ s[n];

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ level1; b̲e̲g̲i̲n̲
                s̲w̲i̲t̲c̲h̲ sw := one, two, three, done;

                i := 0;
    next:       i := i + 1;
                jump(sw, i);
    three:      print(33);
                g̲o̲ t̲o̲ next;
    two:        print(22);
                g̲o̲ t̲o̲ next;
    one:        print(11);
                g̲o̲ t̲o̲ next;
    done:       print(0);
            e̲n̲d̲;

            level1;
        e̲n̲d̲
    )");
    const std::string expect = R"(11
22
33
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, DISABLED_goto_recursive)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ m(i, x, y);
                v̲a̲l̲u̲e̲ i;
                i̲n̲t̲e̲g̲e̲r̲ i;
                l̲a̲b̲e̲l̲ x, y;
            b̲e̲g̲i̲n̲
                i̲f̲ i < 20 t̲h̲e̲n̲ b̲e̲g̲i̲n̲
                    m(i+1, y, pr);
                    g̲o̲t̲o̲ x;
                e̲n̲d̲ e̲l̲s̲e̲ i̲f̲ f̲a̲l̲s̲e̲ t̲h̲e̲n̲
    pr:             print(i);
            e̲n̲d̲;

            m(0, done, done);
    done:
        e̲n̲d̲
    )");
    const std::string expect = R"(17
14
11
8
5
2
)";
    EXPECT_EQ(output, expect);
}
