#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, man_or_boy)
{
    // Only compile for now.
    compile(R"(
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
            print(A(10, 1.0, -1.0, -1.0, 1.0, 0.0));
        e̲n̲d̲
    )");

    // Check symbol table.
    EXPECT_EQ(machine->get_symbol("A"), 2);
    EXPECT_EQ(machine->get_symbol("B"), 10);

    // Check entry address.
    EXPECT_EQ(machine->get_entry(0), 10048);

    //TODO: run and check result
}

//
// Simplified version for easy debug.
//
TEST_F(x1_machine, DISABLED_mob1)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ A(k, x1, x2);
            v̲a̲l̲u̲e̲ k; i̲n̲t̲e̲g̲e̲r̲ k, x1, x2;
            b̲e̲g̲i̲n̲
                i̲n̲t̲e̲g̲e̲r̲ p̲r̲o̲c̲e̲d̲u̲r̲e̲ B;
                b̲e̲g̲i̲n̲
                    k := k - 1;
                    B := A := A(k, B, x1);
                e̲n̲d̲;
                print(k);
                A := i̲f̲ k < 1 t̲h̲e̲n̲ k + x2 e̲l̲s̲e̲ B;
                print(k);
            e̲n̲d̲;
            print(A(1, 1, -1));
        e̲n̲d̲
    )");
    const std::string expect = R"(1
0
0
0
1
)";
    EXPECT_EQ(output, expect);
}
