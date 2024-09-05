#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, input_encoding)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            PRINTTEXT(`0123456789'); NLCR;
            PRINTTEXT(`abcdefghijklmnopqrstuvwxyz'); NLCR;
            PRINTTEXT(`ABCDEFGHIJKLMNOPQRSTUVWXYZ'); NLCR;
            PRINTTEXT(`+-*×/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨@:; ()[]"?'); NLCR;
        e̲n̲d̲
    )");
    // Note: symbols " ? _ are prohibited in strings.
    // Symbols | can be used only in digraphs.

    // Check result in memory.
    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);            // START
    EXPECT_EQ(machine->mem_load(start+3), 0x02'01'00);  // 0 1 2
    EXPECT_EQ(machine->mem_load(start+4), 0x05'04'03);  // 3 4 5
    EXPECT_EQ(machine->mem_load(start+5), 0x08'07'06);  // 6 7 8
    EXPECT_EQ(machine->mem_load(start+6), 0x00'ff'09);  // 9 \377

    EXPECT_EQ(machine->mem_load(start+14), 0x0c'0b'0a);  // a b c
    EXPECT_EQ(machine->mem_load(start+15), 0x0f'0e'0d);  // d e f
    EXPECT_EQ(machine->mem_load(start+16), 0x12'11'10);  // g h i
    EXPECT_EQ(machine->mem_load(start+17), 0x15'14'13);  // j k l
    EXPECT_EQ(machine->mem_load(start+18), 0x18'17'16); // m n o
    EXPECT_EQ(machine->mem_load(start+19), 0x1b'1a'19); // p q r
    EXPECT_EQ(machine->mem_load(start+20), 0x1e'1d'1c); // s t u
    EXPECT_EQ(machine->mem_load(start+21), 0x21'20'1f); // v w x
    EXPECT_EQ(machine->mem_load(start+22), 0xff'23'22); // y z \377

    EXPECT_EQ(machine->mem_load(start+30), 0x27'26'25); // A B C
    EXPECT_EQ(machine->mem_load(start+31), 0x2a'29'28); // D E F
    EXPECT_EQ(machine->mem_load(start+32), 0x2d'2c'2b); // G H I
    EXPECT_EQ(machine->mem_load(start+33), 0x30'2f'2e); // J K L
    EXPECT_EQ(machine->mem_load(start+34), 0x33'32'31); // M N O
    EXPECT_EQ(machine->mem_load(start+35), 0x36'35'34); // P Q R
    EXPECT_EQ(machine->mem_load(start+36), 0x39'38'37); // S T U
    EXPECT_EQ(machine->mem_load(start+37), 0x3c'3b'3a); // V W X
    EXPECT_EQ(machine->mem_load(start+38), 0xff'3e'3d); // Y Z \377

    EXPECT_EQ(machine->mem_load(start+46), 0x42'41'40); // + - *
    EXPECT_EQ(machine->mem_load(start+47), 0x44'43'42); // × / ÷
    EXPECT_EQ(machine->mem_load(start+48), 0x47'46'45); // ↑ > ≥
    EXPECT_EQ(machine->mem_load(start+49), 0x4a'49'48); // = ≤ <
    EXPECT_EQ(machine->mem_load(start+50), 0x4d'4c'4b); // ≠ ¬ ∧
    EXPECT_EQ(machine->mem_load(start+51), 0x50'4f'4e); // ∨ ⊃ ≡
    EXPECT_EQ(machine->mem_load(start+52), 0x59'58'57); // , . ⏨
    EXPECT_EQ(machine->mem_load(start+53), 0x5b'5a'59); // ⏨ : ;
    EXPECT_EQ(machine->mem_load(start+54), 0x63'62'5d); // space ( )
    EXPECT_EQ(machine->mem_load(start+55), 0x79'65'64); // [ ] "
    EXPECT_EQ(machine->mem_load(start+56), 0x00'ff'7a); // ? \377

    // Check output.
    const std::string expect = R"(0123456789
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
+-××/÷↑>≥=≤<≠¬∧∨⊃≡,.⏨⏨:; ()[]"?
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, digraph_encoding)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            PRINTTEXT(``:=''); NLCR;
            PRINTTEXT(``|∧''); NLCR;
            PRINTTEXT(``|=''); NLCR;
            PRINTTEXT(``|<|>''); NLCR;
            PRINTTEXT(``_>''); NLCR;
            PRINTTEXT(``_=''); NLCR;
            PRINTTEXT(``_<''); NLCR;
            PRINTTEXT(``_¬''); NLCR;
            PRINTTEXT(``_:''); NLCR;
        e̲n̲d̲
    )");

    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);            // START
    EXPECT_EQ(machine->mem_load(start+3), 0x67'5c'66);  // := ≔
    EXPECT_EQ(machine->mem_load(start+12), 0x67'45'66); // |∧ ↑
    EXPECT_EQ(machine->mem_load(start+21), 0x67'4b'66); // |= ≠
    EXPECT_EQ(machine->mem_load(start+30), 0x67'66'66); // |< |> ` '
    EXPECT_EQ(machine->mem_load(start+39), 0x67'47'66); // _> ≥
    EXPECT_EQ(machine->mem_load(start+48), 0x67'50'66); // _= ≡
    EXPECT_EQ(machine->mem_load(start+57), 0x67'49'66); // _< ≤
    EXPECT_EQ(machine->mem_load(start+66), 0x67'4f'66); // _¬ ⊃
    EXPECT_EQ(machine->mem_load(start+75), 0x67'44'66); // _: ÷

    const std::string expect = R"(`:='
`↑'
`≠'
``''
`≥'
`≡'
`≤'
`⊃'
`÷'
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, string_quotes)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            PRINTTEXT(`This is a `string'');
            NLCR
        e̲n̲d̲
    )");

    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);           // START
    EXPECT_EQ(machine->mem_load(start+3), 0x12'11'38); // T h i
    EXPECT_EQ(machine->mem_load(start+4), 0x12'5d'1c); // s ␣ i
    EXPECT_EQ(machine->mem_load(start+5), 0x0a'5d'1c); // s ␣ a
    EXPECT_EQ(machine->mem_load(start+6), 0x1c'66'5d); // ␣ ` s
    EXPECT_EQ(machine->mem_load(start+7), 0x12'1b'1d); // t r i
    EXPECT_EQ(machine->mem_load(start+8), 0x67'10'17); // n g '
    EXPECT_EQ(machine->mem_load(start+9), 0x00'00'ff); // \377

    const std::string expect = "This is a `string'\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, virtual_stack)
{
    // Check stack of OPC arguments.
    auto &stack = machine->cpu.stack;

    EXPECT_EQ(stack.count(), 0);
    stack.push_int_value(12345678);
    stack.push_int_addr(12345);
    stack.push_real_value(012'37'64007'36'67'22743ull);
    stack.push_real_addr(23456);
    EXPECT_EQ(stack.count(), 4);

    EXPECT_TRUE(stack.top().is_real_addr());
    EXPECT_EQ(stack.top().get_addr(), 23456);
    stack.pop();
    EXPECT_EQ(stack.count(), 3);

    EXPECT_TRUE(stack.top().is_real_value());
    EXPECT_EQ(stack.top().get_real(), 012'37'64007'36'67'22743ull);
    stack.pop();
    EXPECT_EQ(stack.count(), 2);

    EXPECT_TRUE(stack.top().is_int_addr());
    EXPECT_EQ(stack.top().get_addr(), 12345);
    stack.pop();
    EXPECT_EQ(stack.count(), 1);

    EXPECT_TRUE(stack.top().is_int_value());
    EXPECT_EQ(stack.top().get_int(), 12345678);
    stack.pop();
    EXPECT_EQ(stack.count(), 0);
}

TEST_F(x1_machine, print_integers)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123); NLCR;
            print(-123, 67108863); NLCR;
            NLCR;
            print(-67108863, 0, -0); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(123
-123 67108863

-67108863 0 -0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, print_reals)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123.456); NLCR;
            print(-123.456, 1.615850303564⏨616); NLCR;
            NLCR;
            print(-1.615850303564⏨616, 0.0, -0.0); NLCR;
            print(1.547173023691⏨-617, -1.547173023691⏨-617); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(123.456
-123.456 1.615850303564e+616

-1.615850303564e+616 0 -0
1.547173023691e-617 -1.547173023691e-617
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, print_spaces)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            PRINTTEXT(`/'); SPACE(1); PRINTTEXT(`/'); NLCR;
            PRINTTEXT(`/'); SPACE(2); PRINTTEXT(`/'); NLCR;
            PRINTTEXT(`/'); SPACE(33); PRINTTEXT(`/'); NLCR;
            PRINTTEXT(`/'); SPACE(0); PRINTTEXT(`/'); NLCR;
        e̲n̲d̲
    )");
    const std::string expect = R"(/ /
/  /
/                                 /
//
)";
    EXPECT_EQ(output, expect);
}
