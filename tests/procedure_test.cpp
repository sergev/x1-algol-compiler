#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, procedure_arg0)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ print123();
            b̲e̲g̲i̲n̲
                print(123);
            e̲n̲d̲;
            print123;
        e̲n̲d̲
    )");
    const std::string expect = "123\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_integer_by_value)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ print int(num);
                v̲a̲l̲u̲e̲ num;
                i̲n̲t̲e̲g̲e̲r̲ num;
            b̲e̲g̲i̲n̲
                print(num);
            e̲n̲d̲;
            print int(123);
            print int(-123);
            print int(67108863);
            print int(-67108863);
            print int(0);
            print int(-0);
        e̲n̲d̲
    )");
    const std::string expect = R"(123
-123
67108863
-67108863
0
-0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_integer_by_name)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ i;
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ get int(num);
                i̲n̲t̲e̲g̲e̲r̲ num;
            b̲e̲g̲i̲n̲
                num := -123;
            e̲n̲d̲;
            get int(i);
            print(i);
        e̲n̲d̲
    )");
    const std::string expect = "-123\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_real_by_value)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ print real(num);
                v̲a̲l̲u̲e̲ num;
                r̲e̲a̲l̲ num;
            b̲e̲g̲i̲n̲
                print(num);
            e̲n̲d̲;
            print real(123.456);
            print real(-123.456);
            print real(1.615850303564⏨616);
            print real(-1.615850303564⏨616);
            print real(0.0);
            print real(-0.0);
            print real(1.547173023691⏨-617);
            print real(-1.547173023691⏨-617);
        e̲n̲d̲
    )");
    const std::string expect = R"(123.456
-123.456
1.615850303564e+616
-1.615850303564e+616
0
-0
1.547173023691e-617
-1.547173023691e-617
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_real_by_name)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ i;
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ get real(num);
                r̲e̲a̲l̲ num;
            b̲e̲g̲i̲n̲
                num := -123.456;
            e̲n̲d̲;
            get real(i);
            print(i);
        e̲n̲d̲
    )");
    const std::string expect = "-123.456\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, return_integer)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ div pi(num);
                v̲a̲l̲u̲e̲ num;
                r̲e̲a̲l̲ num;
            b̲e̲g̲i̲n̲
                div pi := entier(num / 3.14159265359);
            e̲n̲d̲;
            print(div pi(9.99));
            print(div pi(-15.5));
            print(div pi(30.7));
        e̲n̲d̲
    )");
    const std::string expect = R"(3
-5
9
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, return_real)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ mul pi(num);
                v̲a̲l̲u̲e̲ num;
                i̲n̲t̲e̲g̲e̲r̲ num;
            b̲e̲g̲i̲n̲
                mul pi := num × 3.14159265359;
            e̲n̲d̲;
            print(mul pi(2));
            print(mul pi(-3));
            print(mul pi(4));
        e̲n̲d̲
    )");
    const std::string expect = R"(6.283185307177
-9.424777960769
12.56637061435
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, real_args)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ args(a, b, c, d, e);
                v̲a̲l̲u̲e̲ a, b, c, d, e;
                r̲e̲a̲l̲ a, b, c, d, e;
            b̲e̲g̲i̲n̲
                print(a, b, c, d, e);
            e̲n̲d̲;
            args(123.456, 1.615850303564⏨616, 0.0,
                 67108863, 1.547173023691⏨-617);
        e̲n̲d̲
    )");
    const std::string expect = R"(123.456
1.615850303564e+616
0
67108863
1.547173023691e-617
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, integer_factorial)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ fact(num);
                v̲a̲l̲u̲e̲ num;
                i̲n̲t̲e̲g̲e̲r̲ num;
            b̲e̲g̲i̲n̲
                fact := i̲f̲ num < 2 t̲h̲e̲n̲ 1 e̲l̲s̲e̲ num × fact(num - 1);
            e̲n̲d̲;
            print(fact(3));
        e̲n̲d̲
    )");
    const std::string expect = R"(6
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, add_formal)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ psum(a, b);
                r̲e̲a̲l̲ a, b;
            b̲e̲g̲i̲n̲
                print(a + b);
            e̲n̲d̲;
            psum(123.456, -98.76);
            psum(-123, 78);
        e̲n̲d̲
    )");
    const std::string expect = R"(24.696
-45
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, add_on_stack)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123 + 456 * 78);
        e̲n̲d̲
    )");
    const std::string expect = "35691\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, subtract_on_stack)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123 - 456 * 78);
        e̲n̲d̲
    )");
    const std::string expect = "-35445\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, divide_on_stack)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123 / (4 / 5));
        e̲n̲d̲
    )");
    const std::string expect = "153.75\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, procedure_PRINTTEXT)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            PRINTTEXT(`Hello `Algol''); NLCR;
            PRINTTEXT(``0123456789''); NLCR;
            PRINTTEXT(``abcdefghijklmnopqrstuvwxyz''); NLCR;
            PRINTTEXT(``ABCDEFGHIJKLMNOPQRSTUVWXYZ''); NLCR;
            PRINTTEXT(``+-×/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨:;:= ''); NLCR;
            PRINTTEXT(``()[]`'''); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(Hello `Algol'
`0123456789'
`abcdefghijklmnopqrstuvwxyz'
`ABCDEFGHIJKLMNOPQRSTUVWXYZ'
`+-×/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨:;:= '
`()[]`''
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, incr_args_by_value)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, r);
            v̲a̲l̲u̲e̲ k, r; i̲n̲t̲e̲g̲e̲r̲ k; r̲e̲a̲l̲ r;
            b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ incr;
                b̲e̲g̲i̲n̲
                    k := k + 1;
                    r := r + 2;
                e̲n̲d̲;
                print(k, r);
                incr;
                print(k, r);
            e̲n̲d̲;
            A(10, 23.5);
        e̲n̲d̲
    )");
    const std::string expect = R"(10
23.5
11
25.5
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, incr_args_by_name)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, r);
            i̲n̲t̲e̲g̲e̲r̲ k; r̲e̲a̲l̲ r;
            b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ incr;
                b̲e̲g̲i̲n̲
                    k := k + 1;
                    r := r + 2;
                e̲n̲d̲;
                print(k, r);
                incr;
                print(k, r);
            e̲n̲d̲;
            A(10, 23.5);
        e̲n̲d̲
    )");
    const std::string expect = R"(10
23.5
11
25.5
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, incr_local_vars)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A;
            b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ k;
                r̲e̲a̲l̲ r;
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ incr;
                b̲e̲g̲i̲n̲
                    k := k + 1;
                    r := r + 2;
                e̲n̲d̲;
                k := 10;
                r := 23.5;
                print(k, r);
                incr;
                print(k, r);
            e̲n̲d̲;
            A;
        e̲n̲d̲
    )");
    const std::string expect = R"(10
23.5
11
25.5
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, integer_var_in_block)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ a;

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(k);
                i̲n̲t̲e̲g̲e̲r̲ k;
            print(k);

            a := 54321;
            pr(a);
            b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ b;
                b := 12345;
                pr(b);
            e̲n̲d̲;
        e̲n̲d̲
    )");
    const std::string expect = R"(54321
12345
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, real_var_in_block)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ a;

            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(k);
                r̲e̲a̲l̲ k;
            print(k);

            a := 543.21;
            pr(a);
            b̲e̲g̲i̲n̲
                r̲e̲a̲l̲ b;
                b := 123.45;
                pr(b);
            e̲n̲d̲;
        e̲n̲d̲
    )");
    const std::string expect = R"(543.21
123.45
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, integer_var_level2)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(x);
                i̲n̲t̲e̲g̲e̲r̲ x;
            b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    print(x);
                e̲n̲d̲;

                B;
            e̲n̲d̲;

            b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ i;
                i := 123;
                A(i);
            e̲n̲d̲;
        e̲n̲d̲
    )");
    const std::string expect = "123\n";
    EXPECT_EQ(output, expect);
}
