#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, arith_add)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(3 + 4); NLCR;
            print(3.25 + 4); NLCR;
            print(3 + 4.625); NLCR;
            print(3.25 + 4.625); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(7
7.25
7.625
7.875
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_subtract)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(3 - 4); NLCR;
            print(3.25 - 4); NLCR;
            print(3 - 4.625); NLCR;
            print(3.25 - 4.625); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(-1
-0.75
-1.625
-1.375
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_multiply_constants)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(3 × 4); NLCR;
            print(3.125 × 4); NLCR;
            print(3 × 4.625); NLCR;
            print(3.25 × 4.625); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(12
12.5
13.875
15.03125
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_multiply_args)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ mul(a, b);
                v̲a̲l̲u̲e̲ a, b;
                r̲e̲a̲l̲ a, b;
            b̲e̲g̲i̲n̲
                mul := a × b;
            e̲n̲d̲;
            print(mul(3, 4)); NLCR;
            print(mul(3.125, -4)); NLCR;
            print(mul(3, 4.625)); NLCR;
            print(mul(3.25, -4.625)); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(12
-12.5
13.875
-15.03125
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_real_divide)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(3 / 4); NLCR;
            print(3.125 / 4); NLCR;
            print(3 / 4.5); NLCR;
            print(3.25 / 4.5); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(0.75
0.78125
0.666666666667
0.7222222222226
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_integer_divide)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(13 ÷ 4); NLCR;
            print(-13 ÷ 4); NLCR;
            print(13 ÷ (-4)); NLCR;
            print(-13 ÷ (-4)); NLCR;
        e̲n̲d̲
)");
    // Note that this fails: print(13 ÷ -4);

    const std::string expect = R"(3
-3
-3
3
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_exponent)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(3 ↑ 4); NLCR;
            print((-2.5) ↑ (-4)); NLCR;
            print(3 ↑ 4.625); NLCR;
            print(2.5 ↑ (-4.625)); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(81
0.0256
160.9480810601
0.01443867895348
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_if_clause)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(i̲f̲ t̲r̲u̲e̲  t̲h̲e̲n̲ 1.5 e̲l̲s̲e̲ 2); NLCR;
            print(i̲f̲ f̲a̲l̲s̲e̲ t̲h̲e̲n̲ 3.5 e̲l̲s̲e̲ 4); NLCR;
            print(i̲f̲ t̲r̲u̲e̲  t̲h̲e̲n̲ 5   e̲l̲s̲e̲ 6.5); NLCR;
            print(i̲f̲ f̲a̲l̲s̲e̲ t̲h̲e̲n̲ 7   e̲l̲s̲e̲ 8.5); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(1.5
4
5
8.5
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, variables)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲ i̲n̲t̲e̲g̲e̲r̲ i; r̲e̲a̲l̲ r;
            i := 1; r := 1.1; i := i + 10; r := r + 10.01;
            print(i, r); NLCR;
            r := i; print(r); NLCR;
            i := 3.14159; print(i); NLCR;
            i := 2.71828; print(i); NLCR;
        e̲n̲d̲
)");
    const std::string expect = R"(11 11.11
11
3
3
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, dynamic_arith)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲ b̲e̲g̲i̲n̲ i̲n̲t̲e̲g̲e̲r̲ i; r̲e̲a̲l̲ r;
            i := 1;
            i := 10 - i;
            i := 1 + i;
            i := 95 / i;
            i := 5 * i;
            r := 1.5;
            r := 10.5 - r;
            r := 1.5 + r;
            r := 94.5 / r;
            r := 5.5 * r;
            print(i, r); NLCR;
        e̲n̲d̲ e̲n̲d̲
)");
    const std::string expect = R"(50 49.5
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, multiply_large_ints)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(1048579 * 1048579); NLCR;
        e̲n̲d̲
)");
    // Note: result differs from 1048579^2 = 1099517919241.
    const std::string expect = "1099517919242\n";
    EXPECT_EQ(output, expect);
}
