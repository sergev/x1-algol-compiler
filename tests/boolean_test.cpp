#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, boolean_operations)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(_f_a_l_s_e);
            print(_t_r_u_e);
            print;
            print(¬ _f_a_l_s_e);
            print(¬ _t_r_u_e);
            print;
            print(_f_a_l_s_e ∧ _f_a_l_s_e);
            print(_f_a_l_s_e ∧ _t_r_u_e);
            print(_t_r_u_e   ∧ _f_a_l_s_e);
            print(_t_r_u_e   ∧ _t_r_u_e);
            print;
            print(_f_a_l_s_e ∨ _f_a_l_s_e);
            print(_f_a_l_s_e ∨ _t_r_u_e);
            print(_t_r_u_e   ∨ _f_a_l_s_e);
            print(_t_r_u_e   ∨ _t_r_u_e);
            print;
            print(_f_a_l_s_e ⊃ _f_a_l_s_e);
            print(_f_a_l_s_e ⊃ _t_r_u_e);
            print(_t_r_u_e   ⊃ _f_a_l_s_e);
            print(_t_r_u_e   ⊃ _t_r_u_e);
            print;
            print(_f_a_l_s_e ≡ _f_a_l_s_e);
            print(_f_a_l_s_e ≡ _t_r_u_e);
            print(_t_r_u_e   ≡ _f_a_l_s_e);
            print(_t_r_u_e   ≡ _t_r_u_e);
        _e_n_d
    )");
    const std::string expect = R"(1
0

0
1

1
1
1
0

1
0
0
0

0
0
1
0

0
1
1
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, relations)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(3    < 3.14);
            print(3    < 3);
            print(3.14 < 3);
            print(3.14 < 3.14);
            print;
            print(3    ≤ 3.14);
            print(3    ≤ 3);
            print(3.14 ≤ 3);
            print(3.14 ≤ 3.14);
            print;
            print(3    = 3.14);
            print(3    = 3);
            print(3.14 = 3);
            print(3.14 = 3.14);
            print;
            print(3    ≥ 3.14);
            print(3    ≥ 3);
            print(3.14 ≥ 3);
            print(3.14 ≥ 3.14);
            print;
            print(3    > 3.14);
            print(3    > 3);
            print(3.14 > 3);
            print(3.14 > 3.14);
            print;
            print(3    ≠ 3.14);
            print(3    ≠ 3);
            print(3.14 ≠ 3);
            print(3.14 ≠ 3.14);
        _e_n_d
)");
    const std::string expect = R"(0
1
1
1

0
0
1
0

1
0
1
0

1
0
0
0

1
1
0
1

0
1
0
1
)";
    EXPECT_EQ(output, expect);
}