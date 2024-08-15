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
