#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, array_single_dim)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ a̲r̲r̲a̲y̲ a[1:10];
            a[1] := 1;
            a[10] := a[1] + 9;
            print(a[1]);
            print(a[10]);
        e̲n̲d̲
)");
    const std::string expect = R"(1
10
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, array_multi_dim)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ a̲r̲r̲a̲y̲ a[1:10,2:9,3:8,4:7];
            a[1,2,3,4] := 1234;
            a[10,9,8,7] := a[1,2,3,4] + 5678;
            print(a[1,2,3,4]);
            print(a[10,9,8,7]);
        e̲n̲d̲
)");
    const std::string expect = R"(1234
6912
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, array_negative_bound)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ a̲r̲r̲a̲y̲ a[-1:1,-1:1];
            a[0,0] := 1234;
            a[-1,1] := a[1,-1] := 5678;
            print(a[-1,1], a[0,0], a[1,-1]);
            print(a[-6,2]);
        e̲n̲d̲
)");
    const std::string expect = R"(5678
1234
5678
1234
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, array_local_real)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n _i_n_t i;
            _b_e_g_i_n _i_n_t a;
                _r_e_a_l _a_r_r_a_y b[-1:11];
                _i_n_t z;
                a := 12345;
                z := 54321;
                b[-1] := -1.5;
                b[11] := 11.25;
                print(a);
                print(z);
                i := -1;
                loop: _i_f i < 12 _t_h_e_n _b_e_g_i_n
                    print(b[i]);
                    i := i + 4;
                    _g_o_t_o loop
                _e_n_d
            _e_n_d
         _e_n_d
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
