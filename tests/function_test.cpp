#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, function_abs)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(abs(-123.456)); NLCR;
            print(abs(123.456)); NLCR;
            print(abs(-1.615850303564⏨616)); NLCR;
            print(abs(1.615850303564⏨616)); NLCR;
            print(abs(-0.0)); NLCR;
            print(abs(0.0)); NLCR;
            print(abs(-67108863)); NLCR;
            print(abs(67108863)); NLCR;
            print(abs(-0)); NLCR;
            print(abs(0)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(123.456
123.456
1.615850303564e+616
1.615850303564e+616
0
0
67108863
67108863
0
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sign)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(sign(-123.456)); NLCR;
            print(sign(123.456)); NLCR;
            print(sign(-1.615850303564⏨616)); NLCR;
            print(sign(1.615850303564⏨616)); NLCR;
            print(sign(-0.0)); NLCR;
            print(sign(0.0)); NLCR;
            print(sign(-1)); NLCR;
            print(sign(1)); NLCR;
            print(sign(-67108863)); NLCR;
            print(sign(67108863)); NLCR;
            print(sign(-0)); NLCR;
            print(sign(0)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(-1
1
-1
1
0
0
-1
1
-1
1
0
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sqrt)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(sqrt(2)); NLCR;
            print(sqrt(99999)); NLCR;
            print(sqrt(1.01)); NLCR;
            print(sqrt(0.99)); NLCR;
            print(sqrt(0.0003)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(1.414213562373
316.2261848738
1.004987562112
0.9949874371068
0.0173205080757
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sin)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(sin(0.0001)); NLCR;
            print(sin(-0.0001)); NLCR;
            print(sin(0.7854)); NLCR;
            print(sin(-0.7854)); NLCR;
            print(sin(1.5)); NLCR;
            print(sin(-1.6)); NLCR;
            print(sin(3141.6)); NLCR;
            print(sin(-3141.6)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(9.999999983334e-05
-9.999999983334e-05
0.7071080798596
-0.7071080798596
0.9974949866037
-0.9995736030414
0.007346345616369
-0.007346345616369
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_cos)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(cos(0.01)); NLCR;
            print(cos(-0.01)); NLCR;
            print(cos(0.7854)); NLCR;
            print(cos(-0.7854)); NLCR;
            print(cos(1.5)); NLCR;
            print(cos(-1.6)); NLCR;
            print(cos(3141.6)); NLCR;
            print(cos(-3141.6)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(0.9999500004169
0.9999500004169
0.707105482511
0.707105482511
0.07073720166773
-0.02919952230164
0.9999730152385
0.9999730152385
)";
    EXPECT_EQ(output, expect);
}

//
// Cannot compile this program: it wants a library tape.
// "Need MCP 3"
// "Need MCP 4"
//
TEST_F(x1_machine, function_arctan)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(arctan(0.0001)); NLCR;
            print(arctan(-0.0001)); NLCR;
            print(arctan(0.5)); NLCR;
            print(arctan(-0.5)); NLCR;
            print(arctan(2)); NLCR;
            print(arctan(-2)); NLCR;
            print(arctan(999.9)); NLCR;
            print(arctan(-999.9)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(9.99999996667e-05
-9.99999996667e-05
0.4636476090009
-0.4636476090009
1.107148717794
-1.107148717794
1.569796227119
-1.569796227119
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_ln)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(ln(0.0001)); NLCR;
            print(ln(0.5)); NLCR;
            print(ln(0.9999)); NLCR;
            print(ln(1.0001)); NLCR;
            print(ln(2)); NLCR;
            print(ln(4)); NLCR;
            print(ln(8)); NLCR;
            print(ln(99999)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(-9.210340371981
-0.6931471805601
-0.0001000050005356
9.999499962621e-05
0.6931471805601
1.38629436112
2.079441541679
11.51291546492
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_exp)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(exp(-6)); NLCR;
            print(exp(-1)); NLCR;
            print(exp(0.0001)); NLCR;
            print(exp(1)); NLCR;
            print(exp(6)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(0.002478752176668
0.3678794411712
1.000100005
2.71828182846
403.4287934927
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_entier)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(entier(-98.7777)); NLCR;
            print(entier(98.7777)); NLCR;
            print(entier(-5.4444)); NLCR;
            print(entier(5.4444)); NLCR;
            print(entier(-0.9999)); NLCR;
            print(entier(0.9999)); NLCR;
            print(entier(-0.1111)); NLCR;
            print(entier(0.1111)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(-99
98
-6
5
-1
0
-1
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_EVEN)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(EVEN(0), EVEN(1), EVEN(-0), EVEN(-1)); NLCR;
            print(EVEN(2), EVEN(3), EVEN(22), EVEN(33)); NLCR;
            print(EVEN(-2), EVEN(-3), EVEN(-22), EVEN(-33)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(1 -1 1 -1
1 -1 1 -1
1 -1 1 -1
)";
    EXPECT_EQ(output, expect);
}
