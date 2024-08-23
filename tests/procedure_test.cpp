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

TEST_F(x1_machine, formal_arithmetic)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ arith(a, b, c, d, e);
                r̲e̲a̲l̲ a, b, c, d, e;
            b̲e̲g̲i̲n̲
                arith := (a + b) * (c - d) / e;
            e̲n̲d̲;
            print(arith(1.1, 2.2, 3.3, 4, 5));
            print(arith(-1, 2+3, 4*5, 6/7, 8.8));
        e̲n̲d̲
    )");
    const std::string expect = R"(-0.4620000000009
8.7012987013
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

TEST_F(x1_machine, integer_args_level7)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(m1); i̲n̲t̲e̲g̲e̲r̲ m1; b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ B(m2); i̲n̲t̲e̲g̲e̲r̲ m2; b̲e̲g̲i̲n̲
                    p̲r̲o̲c̲e̲d̲u̲r̲e̲ C(m3); i̲n̲t̲e̲g̲e̲r̲ m3; b̲e̲g̲i̲n̲
                        p̲r̲o̲c̲e̲d̲u̲r̲e̲ D(m4); i̲n̲t̲e̲g̲e̲r̲ m4; b̲e̲g̲i̲n̲
                            p̲r̲o̲c̲e̲d̲u̲r̲e̲ E(m5); i̲n̲t̲e̲g̲e̲r̲ m5; b̲e̲g̲i̲n̲
                                p̲r̲o̲c̲e̲d̲u̲r̲e̲ F(m6); i̲n̲t̲e̲g̲e̲r̲ m6; b̲e̲g̲i̲n̲
                                    p̲r̲o̲c̲e̲d̲u̲r̲e̲ G(m7); i̲n̲t̲e̲g̲e̲r̲ m7; b̲e̲g̲i̲n̲
                                        PRINTTEXT(`G'); NLCR;
                                        print(m1, m2, m3, m4, m5, m6, m7);
                                    e̲n̲d̲ G;
                                    PRINTTEXT(`F'); NLCR;
                                    print(m1, m2, m3, m4, m5, m6);
                                    G(m6 - 1);
                                e̲n̲d̲ F;
                                PRINTTEXT(`E'); NLCR;
                                print(m1, m2, m3, m4, m5);
                                F(m5 - 1);
                            e̲n̲d̲ E;
                            PRINTTEXT(`D'); NLCR;
                            print(m1, m2, m3, m4);
                            E(m4 - 1);
                        e̲n̲d̲ D;
                        PRINTTEXT(`C'); NLCR;
                        print(m1, m2, m3);
                        D(m3 - 1);
                    e̲n̲d̲ C;
                    PRINTTEXT(`B'); NLCR;
                    print(m1, m2);
                    C(m2 - 1);
                e̲n̲d̲ B;
                PRINTTEXT(`A'); NLCR;
                print(m1);
                B(m1 - 1);
            e̲n̲d̲ A;
            A(-1);
        e̲n̲d̲
    )");
    const std::string expect = R"(A
-1
B
-1
-2
C
-1
-2
-3
D
-1
-2
-3
-4
E
-1
-2
-3
-4
-5
F
-1
-2
-3
-4
-5
-6
G
-1
-2
-3
-4
-5
-6
-7
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg_from_previous_level)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(x); i̲n̲t̲e̲g̲e̲r̲ x; b̲e̲g̲i̲n̲
                print(x);
            e̲n̲d̲;
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(x); i̲n̲t̲e̲g̲e̲r̲ x; b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ B; b̲e̲g̲i̲n̲
                    pr(x);
                e̲n̲d̲;
                pr(x);
                B;
            e̲n̲d̲;
            A(-123);
        e̲n̲d̲
    )");
    const std::string expect = R"(-123
-123
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, getters_level2)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ pr(k, n); v̲a̲l̲u̲e̲ k, n; i̲n̲t̲e̲g̲e̲r̲ k, n; b̲e̲g̲i̲n̲
                print(k, n);
            e̲n̲d̲;
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k); v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k; b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ getk; b̲e̲g̲i̲n̲
                    getk := k;
                e̲n̲d̲;
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ B(n); v̲a̲l̲u̲e̲ n; i̲n̲t̲e̲g̲e̲r̲ n; b̲e̲g̲i̲n̲
                    i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ getn; b̲e̲g̲i̲n̲
                        getn := n;
                    e̲n̲d̲;
                    print(k, n);
                    pr(k, n);
                    pr(getk, getn);
                e̲n̲d̲;
                B(456);
            e̲n̲d̲;
            A(123);
        e̲n̲d̲
    )");
    const std::string expect = R"(123
456
123
456
123
456
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg_address_level2)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲̲e̲d̲u̲r̲e̲ c(x); i̲n̲t̲̲e̲g̲e̲r̲ x; b̲e̲g̲i̲n̲
                i̲n̲t̲̲e̲g̲e̲r̲ y;
                p̲r̲o̲c̲ d(x); i̲n̲t̲ x;
                    x := 12345;
                d(x);
            e̲n̲d̲;
            p̲r̲o̲c̲̲e̲d̲u̲r̲e̲ a; b̲e̲g̲i̲n̲
                i̲n̲t̲̲e̲g̲e̲r̲ x;
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ b(x); i̲n̲t̲̲e̲g̲e̲r̲ x; b̲e̲g̲i̲n̲
                    i̲n̲t̲̲̲e̲g̲e̲r̲ y;
                    c(x)
                e̲n̲d̲;
                b(x);
                print(x);
            e̲n̲d̲;
            a;
        e̲n̲d̲
    )");
    const std::string expect = "12345\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg_by_name_recursive)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲̲e̲g̲e̲r̲̲ p̲r̲o̲c̲̲e̲d̲u̲r̲e̲ bar(a); i̲n̲t̲̲e̲g̲e̲r̲ a; b̲e̲g̲i̲n̲
                bar := a
            e̲n̲d̲;
            print(bar(bar(123)));
        e̲n̲d̲
    )");
    const std::string expect = "123\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, args_by_value_level1)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            c̲o̲m̲m̲e̲n̲t̲ v̲a̲l̲u̲e̲ is needed for the bug ;
            i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ bar(a, b); v̲a̲l̲u̲e̲ a, b; i̲n̲t̲e̲g̲e̲r̲ a, b; b̲e̲g̲i̲n̲
                PRINTTEXT(`bar'); NLCR;
                bar := 123;
            e̲n̲d̲;
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ foo; b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ a, b;
                a := bar(bar(b, 0) - bar(a, 0), 0);
                print(a);
            e̲n̲d̲;
            foo;
        e̲n̲d̲
    )");
    const std::string expect = R"(bar
bar
bar
123
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, DISABLED_easter_sunday)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
        i̲n̲t̲e̲g̲e̲r̲ year, month, day;

           i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ rem(a, b);
           v̲a̲l̲u̲e̲ a, b; i̲n̲t̲e̲g̲e̲r̲ a, b;
              b̲e̲g̲i̲n̲ i̲n̲t̲e̲g̲e̲r̲ c;
              c := a _: b; rem := a - b * c
              e̲n̲d̲ rem;

        i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ easter sunday(y);
        v̲a̲l̲u̲e̲ y; i̲n̲t̲e̲g̲e̲r̲ y;
           b̲e̲g̲i̲n̲ i̲n̲t̲e̲g̲e̲r̲ a, b, c, h;
           a := rem(y, 19);
           b := y _: 100; c := rem(y, 100);
           h := rem((19 * a + b - b _: 4 + 15 - ((8 * b + 13) _: 25)), 30);
           _c c := rem((2 * (b-b_:4*4 + c _: 4) - h - (c-c_:4*4) + 32), 7) works;
           c := rem((2 * (rem(b, 4) + c _: 4) - h - rem(c, 4) + 32), 7);
           h := h + c - 7 * ((a + 11 * h + 19 * c) _: 433);
           a := (h + 90) _: 25;
           easter sunday := rem(33 * a + h + 19, 32);
           e̲n̲d̲ easter sunday;

           f̲o̲r̲ year := 2010 s̲t̲e̲p̲ 1 u̲n̲t̲i̲l̲ 2030 d̲o̲
           b̲e̲g̲i̲n̲
               day := easter sunday(year);
               print(year);
               i̲f̲ day < 25 t̲h̲e̲n̲ b̲e̲g̲i̲n̲  PRINTTEXT(` April '); print(day) e̲n̲d̲
               e̲l̲s̲e̲ b̲e̲g̲i̲n̲ PRINTTEXT(` March '); print(day) e̲n̲d̲;
           e̲n̲d̲
        e̲n̲d̲
)");
    const std::string expect = R"(2010
 April 4
2011
 April 24
2012
 April 8
2013
 March 31
2014
 April 20
2015
 April 5
2016
 March 27
2017
 April 16
2018
 April 1
2019
 April 21
2020
 April 12
2021
 April 4
2022
 April 17
2023
 April 9
2024
 March 31
2025
 April 20
2026
 April 5
2027
 March 28
2028
 April 16
2029
 April 1
2030
 April 21
)";
    EXPECT_EQ(output, expect);
}
