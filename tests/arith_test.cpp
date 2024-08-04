#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, arith_add)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(3 + 4);
        _e_n_d
)");
    const std::string expect = R"(7
)";
    EXPECT_EQ(output, expect);
}

//TODO: + − × / ÷ ↑

//TODO: (if a then b else c)
