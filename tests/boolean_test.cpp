#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, boolean_operations)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(f̲a̲l̲s̲e̲);
            print(t̲r̲u̲e̲);
            print;
            print(¬ f̲a̲l̲s̲e̲);
            print(¬ t̲r̲u̲e̲);
            print;
            print(f̲a̲l̲s̲e̲ ∧ f̲a̲l̲s̲e̲);
            print(f̲a̲l̲s̲e̲ ∧ t̲r̲u̲e̲);
            print(t̲r̲u̲e̲  ∧ f̲a̲l̲s̲e̲);
            print(t̲r̲u̲e̲  ∧ t̲r̲u̲e̲);
            print;
            print(f̲a̲l̲s̲e̲ ∨ f̲a̲l̲s̲e̲);
            print(f̲a̲l̲s̲e̲ ∨ t̲r̲u̲e̲);
            print(t̲r̲u̲e̲  ∨ f̲a̲l̲s̲e̲);
            print(t̲r̲u̲e̲  ∨ t̲r̲u̲e̲);
            print;
            print(f̲a̲l̲s̲e̲ ⊃ f̲a̲l̲s̲e̲);
            print(f̲a̲l̲s̲e̲ ⊃ t̲r̲u̲e̲);
            print(t̲r̲u̲e̲  ⊃ f̲a̲l̲s̲e̲);
            print(t̲r̲u̲e̲  ⊃ t̲r̲u̲e̲);
            print;
            print(f̲a̲l̲s̲e̲ ≡ f̲a̲l̲s̲e̲);
            print(f̲a̲l̲s̲e̲ ≡ t̲r̲u̲e̲);
            print(t̲r̲u̲e̲  ≡ f̲a̲l̲s̲e̲);
            print(t̲r̲u̲e̲  ≡ t̲r̲u̲e̲);
        e̲n̲d̲
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
        b̲e̲g̲i̲n̲
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
        e̲n̲d̲
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
