#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, loop_value_list)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t_e_g_e_r i,j;
            _f_o_r i := 1,2,3 _d_o
                _f_o_r j := 1,2,3 _d_o _b_e_g_i_n
                    print(10*i+j); NLCR;
                _e_n_d
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
                          i*2 _w_h_i_l_e i < 100 _d_o _b_e_g_i_n
                    print(i); NLCR;
            _e_n_d
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
                          i*2 _s_t_e_p -3 _u_n_t_i_l 1 _d_o _b_e_g_i_n
                    print(i); NLCR;
            _e_n_d
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
                print(a[i]); NLCR;
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

TEST_F(x1_machine, loop_locarray)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            b̲e̲g̲i̲n̲ i̲n̲t̲ a;
                r̲e̲a̲l̲ a̲r̲r̲a̲y̲ b[-1:11];
                i̲n̲t̲ z, i;
                a := 12345;
                z := 54321;
                b[-1] := -1.5;
                b[11] := 11.25;
                print(a); NLCR;
                print(z); NLCR;
                f̲o̲r̲ i := -1 s̲t̲e̲p̲ 4 u̲n̲t̲i̲l̲ 11 d̲o̲
                b̲e̲g̲i̲n̲
                    print(b[i]); NLCR;
                e̲n̲d̲
            e̲n̲d̲
        e̲n̲d̲
)");
    const std::string expect = R"(12345
54321
-1.5
0
0
11.25
)";
    EXPECT_EQ(output, expect);
}
