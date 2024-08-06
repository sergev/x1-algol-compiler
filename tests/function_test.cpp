#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, function_abs)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(abs(-123.456));
            print(abs(123.456));
            print(abs(-1.615850303564⏨616));
            print(abs(1.615850303564⏨616));
            print(abs(-0.0));
            print(abs(0.0));
            print(abs(-67108863));
            print(abs(67108863));
            print(abs(-0));
            print(abs(0));
        _e_n_d
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
        _b_e_g_i_n
            print(sign(-123.456));
            print(sign(123.456));
            print(sign(-1.615850303564⏨616));
            print(sign(1.615850303564⏨616));
            print(sign(-0.0));
            print(sign(0.0));
            print(sign(-1));
            print(sign(1));
            print(sign(-67108863));
            print(sign(67108863));
            print(sign(-0));
            print(sign(0));
        _e_n_d
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
        _b_e_g_i_n
            print(sqrt(2));
            print(sqrt(99999));
            print(sqrt(1.01));
            print(sqrt(0.99));
            print(sqrt(0.0003));
        _e_n_d
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
        _b_e_g_i_n
            print(sin(0.0001));
            print(sin(-0.0001));
            print(sin(0.7854));
            print(sin(-0.7854));
            print(sin(1.5));
            print(sin(-1.6));
            print(sin(3141.6));
            print(sin(-3141.6));
        _e_n_d
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
        _b_e_g_i_n
            print(cos(0.01));
            print(cos(-0.01));
            print(cos(0.7854));
            print(cos(-0.7854));
            print(cos(1.5));
            print(cos(-1.6));
            print(cos(3141.6));
            print(cos(-3141.6));
        _e_n_d
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
TEST_F(x1_machine, DISABLED_function_arctan)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(arctan(0.0001));
            print(arctan(-0.0001));
            print(arctan(0.5));
            print(arctan(-0.5));
            print(arctan(2));
            print(arctan(-2));
            print(arctan(999.9));
            print(arctan(-999.9));
        _e_n_d
    )");
    const std::string expect = R"(0.000100
-0.000100
0.463648
-0.463648
1.107149
-1.107149
1.569796
-1.569796
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_ln)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(ln(0.0001));
            print(ln(0.5));
            print(ln(0.9999));
            print(ln(1.0001));
            print(ln(2));
            print(ln(4));
            print(ln(8));
            print(ln(99999));
        _e_n_d
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
        _b_e_g_i_n
            print(exp(-6));
            print(exp(-1));
            print(exp(0.0001));
            print(exp(1));
            print(exp(6));
        _e_n_d
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
        _b_e_g_i_n
            print(entier(-98.7777));
            print(entier(98.7777));
            print(entier(-5.4444));
            print(entier(5.4444));
            print(entier(-0.9999));
            print(entier(0.9999));
            print(entier(-0.1111));
            print(entier(0.1111));
        _e_n_d
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
