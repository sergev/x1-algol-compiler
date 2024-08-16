#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, loop_value_list)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t_e_g_e_r i,j;
            _f_o_r i := 1,2,3 _d_o
                _f_o_r j := 1,2,3 _d_o
                    print(10*i+j)
        _e_n_d
)");
    const std::string expect = R"(11
12
13
21
22
23
31
32
33
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, loop_while)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t_e_g_e_r i,j;
            _f_o_r i := 1,i+1 _w_h_i_l_e i < 6,
                          i*2 _w_h_i_l_e i < 100 _d_o
                    print(i)
        _e_n_d
)");
    const std::string expect = R"(1
2
3
4
5
12
24
48
96
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, loop_step)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t_e_g_e_r i,j;
            _f_o_r i := 1 _s_t_e_p 1 _u_n_t_i_l 5,
                          i*2 _s_t_e_p -3 _u_n_t_i_l 1 _d_o
                    print(i)
        _e_n_d
)");
    const std::string expect = R"(1
2
3
4
5
12
9
6
3
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, loop_fibonacci)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t_e_g_e_r i;
            _i_n_t_e_g_e_r _a_r_r_a_y a[0:1];
            a[0] := 0; i := 1;
            _f_o_r a[i] := 1 _s_t_e_p a[1-i] _u_n_t_i_l 100 _d_o
            _b_e_g_i_n
                print(a[i]);
                i := 1-i;
            _e_n_d
        _e_n_d
)");
    const std::string expect = R"(1
1
2
3
5
8
13
21
34
55
89
)";
    EXPECT_EQ(output, expect);
}
