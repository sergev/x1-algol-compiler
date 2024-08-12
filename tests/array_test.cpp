#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, array_single_dim)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _r_e_a_l _a_r_r_a_y a[1:10];
            a[1] := 1;
            a[10] := a[1] + 9;
            print(a[1]);
            print(a[10]);
        _e_n_d
)");
    const std::string expect = R"(1
10
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, array_multi_dim)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _i_n_t_e_g_e_r _a_r_r_a_y a[1:10,2:9,3:8,4:7];
            a[1,2,3,4] := 1234;
            a[10,9,8,7] := a[1,2,3,4] + 5678;
            print(a[1,2,3,4]);
            print(a[10,9,8,7]);
        _e_n_d
)");
    const std::string expect = R"(1234
6912
)";
    EXPECT_EQ(output, expect);
}

