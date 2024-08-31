#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, man_or_boy)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x1, x2, x3, x4, x5);
            v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k;
            r̲e̲a̲l̲ x1, x2, x3, x4, x5;
            b̲e̲g̲i̲n̲
                r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    k := k - 1;
                    B := A := A(k, B, x1, x2, x3, x4);
                e̲n̲d̲;
                A := i̲f̲ k < 1 t̲h̲e̲n̲ x4 + x5 e̲l̲s̲e̲ B;
            e̲n̲d̲;
            print(A(10, 1.0, -1.0, -1.0, 1.0, 0.0)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = "-67\n";
    EXPECT_EQ(output, expect);

    // Check symbol table.
    EXPECT_EQ(machine->get_symbol("A"), 2);
    EXPECT_EQ(machine->get_symbol("B"), 10);
}

//
// Simplified version for easy debug.
//
TEST_F(x1_machine, mob1)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x1, x2);
            v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k, x1, x2;
            b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    print(k); NLCR;
                    k := k - 1;
                    print(k); NLCR;
                    B := A(k, B, x1);
                e̲n̲d̲;
                A := i̲f̲ k < 1 t̲h̲e̲n̲ x2 e̲l̲s̲e̲ B;
            e̲n̲d̲;
            print(A(1, 1, -1)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(1
0
1
)";
    EXPECT_EQ(output, expect);
}

//
// Even more simplified version.
//
TEST_F(x1_machine, mob0)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x);
                v̲a̲l̲u̲e̲ k;
                i̲n̲t̲e̲g̲e̲r̲ k, x;
            b̲e̲g̲i̲n̲
                p̲r̲o̲c̲e̲d̲u̲r̲e̲ B(y);
                    i̲n̲t̲e̲g̲e̲r̲ y;
                b̲e̲g̲i̲n̲
                    print(k, x, y); NLCR;
                    i̲f̲ x > -10 t̲h̲e̲n̲ A(456, -10);
                    i̲f̲ y < 0 t̲h̲e̲n̲ B(20);
                e̲n̲d̲ B;

                print(k, x); NLCR;
                B(x - 1);
            e̲n̲d̲ A;
            A(123, -1);
        e̲n̲d̲
    )");
    const std::string expect = R"(123 -1
123 -1 -2
456 -10
456 -10 -11
456 -10 20
123 -1 20
456 -10
456 -10 -11
456 -10 20
)";
    EXPECT_EQ(output, expect);
}

//
// Two levels of recursion.
//
TEST_F(x1_machine, mob2)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x1, x2, x3, x4, x5);
            v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k;
            r̲e̲a̲l̲ x1, x2, x3, x4, x5;
            b̲e̲g̲i̲n̲
                r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    k := k - 1;
                    print(k); NLCR;
                    B := A(k, B, x1, x2, x3, x4);
                e̲n̲d̲;
                A := i̲f̲ k < 1 t̲h̲e̲n̲ x4 + x5 e̲l̲s̲e̲ B;
            e̲n̲d̲;
            print(A(2, 11.0, 22.0, 33.0, 44.0, 0.0)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(1
0
55
)";
    EXPECT_EQ(output, expect);
}

//
// Four levels of recursion.
//
TEST_F(x1_machine, mob4)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x1, x2, x3, x4, x5);
            v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k;
            r̲e̲a̲l̲ x1, x2, x3, x4, x5;
            b̲e̲g̲i̲n̲
                r̲e̲a̲l̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    k := k - 1;
                    print(k); NLCR;
                    B := A(k, B, x1, x2, x3, x4);
                e̲n̲d̲;
                A := i̲f̲ k < 1 t̲h̲e̲n̲ x4 + x5 e̲l̲s̲e̲ B;
            e̲n̲d̲;
            print(A(4, 11.0, 22.0, 33.0, 44.0, 0.0)); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(3
2
1
0
2
1
0
44
)";
    EXPECT_EQ(output, expect);
}
