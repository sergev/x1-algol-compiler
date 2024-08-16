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
