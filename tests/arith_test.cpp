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

//TODO: − × / ÷ ↑

//TODO: (if a then b else c)
