#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, arith_add)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(3 + 4);
            print(3.25 + 4);
            print(3 + 4.625);
            print(3.25 + 4.625);
        _e_n_d
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
        _b_e_g_i_n
            print(3 - 4);
            print(3.25 - 4);
            print(3 - 4.625);
            print(3.25 - 4.625);
        _e_n_d
)");
    const std::string expect = R"(-1
-0.75
-1.625
-1.375
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_multiply)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(3 × 4);
            print(3.125 × 4);
            print(3 × 4.625);
            print(3.25 × 4.625);
        _e_n_d
)");
    const std::string expect = R"(12
12.5
13.875
15.03125
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arith_real_divide)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(3 / 4);
            print(3.125 / 4);
            print(3 / 4.5);
            print(3.25 / 4.5);
        _e_n_d
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
        _b_e_g_i_n
            print(13 ÷ 4);
            print(-13 ÷ 4);
            print(13 ÷ (-4));
            print(-13 ÷ (-4));
        _e_n_d
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
        _b_e_g_i_n
            print(3 ↑ 4);
            print((-2.5) ↑ (-4));
            print(3 ↑ 4.625);
            print(2.5 ↑ (-4.625));
        _e_n_d
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
        _b_e_g_i_n
            print(_i_f _t_r_u_e   _t_h_e_n 1.5 _e_l_s_e 2);
            print(_i_f _f_a_l_s_e _t_h_e_n 3.5 _e_l_s_e 4);
            print(_i_f _t_r_u_e   _t_h_e_n 5   _e_l_s_e 6.5);
            print(_i_f _f_a_l_s_e _t_h_e_n 7   _e_l_s_e 8.5);
        _e_n_d
)");
    const std::string expect = R"(1.5
4
5
8.5
)";
    EXPECT_EQ(output, expect);
}
