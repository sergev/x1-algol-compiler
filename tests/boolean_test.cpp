#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, boolean_operations)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(f̲a̲l̲s̲e̲); NLCR;
            print(t̲r̲u̲e̲); NLCR;
            NLCR;
            print(¬ f̲a̲l̲s̲e̲); NLCR;
            print(¬ t̲r̲u̲e̲); NLCR;
            NLCR;
            print(f̲a̲l̲s̲e̲ ∧ f̲a̲l̲s̲e̲); NLCR;
            print(f̲a̲l̲s̲e̲ ∧ t̲r̲u̲e̲); NLCR;
            print(t̲r̲u̲e̲  ∧ f̲a̲l̲s̲e̲); NLCR;
            print(t̲r̲u̲e̲  ∧ t̲r̲u̲e̲); NLCR;
            NLCR;
            print(f̲a̲l̲s̲e̲ ∨ f̲a̲l̲s̲e̲); NLCR;
            print(f̲a̲l̲s̲e̲ ∨ t̲r̲u̲e̲); NLCR;
            print(t̲r̲u̲e̲  ∨ f̲a̲l̲s̲e̲); NLCR;
            print(t̲r̲u̲e̲  ∨ t̲r̲u̲e̲); NLCR;
            NLCR;
            print(f̲a̲l̲s̲e̲ ⊃ f̲a̲l̲s̲e̲); NLCR;
            print(f̲a̲l̲s̲e̲ ⊃ t̲r̲u̲e̲); NLCR;
            print(t̲r̲u̲e̲  ⊃ f̲a̲l̲s̲e̲); NLCR;
            print(t̲r̲u̲e̲  ⊃ t̲r̲u̲e̲); NLCR;
            NLCR;
            print(f̲a̲l̲s̲e̲ ≡ f̲a̲l̲s̲e̲); NLCR;
            print(f̲a̲l̲s̲e̲ ≡ t̲r̲u̲e̲); NLCR;
            print(t̲r̲u̲e̲  ≡ f̲a̲l̲s̲e̲); NLCR;
            print(t̲r̲u̲e̲  ≡ t̲r̲u̲e̲); NLCR;
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
            print(3    < 3.14); NLCR;
            print(3    < 3); NLCR;
            print(3.14 < 3); NLCR;
            print(3.14 < 3.14); NLCR;
            NLCR;
            print(3    ≤ 3.14); NLCR;
            print(3    ≤ 3); NLCR;
            print(3.14 ≤ 3); NLCR;
            print(3.14 ≤ 3.14); NLCR;
            NLCR;
            print(3    = 3.14); NLCR;
            print(3    = 3); NLCR;
            print(3.14 = 3); NLCR;
            print(3.14 = 3.14); NLCR;
            NLCR;
            print(3    ≥ 3.14); NLCR;
            print(3    ≥ 3); NLCR;
            print(3.14 ≥ 3); NLCR;
            print(3.14 ≥ 3.14); NLCR;
            NLCR;
            print(3    > 3.14); NLCR;
            print(3    > 3); NLCR;
            print(3.14 > 3); NLCR;
            print(3.14 > 3.14); NLCR;
            NLCR;
            print(3    ≠ 3.14); NLCR;
            print(3    ≠ 3); NLCR;
            print(3.14 ≠ 3); NLCR;
            print(3.14 ≠ 3.14); NLCR;
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
