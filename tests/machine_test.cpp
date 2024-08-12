#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, input_encoding)
{
    // Technically it's a wrong program, as print() accepts only
    // a number on input. But for checking encoding it's all right.
    // We are not going to run it.
    compile(R"(
        b̲e̲g̲i̲n̲
            print(`0123456789');
            print(`abcdefghijklmnopqrstuvwxyz');
            print(`ABCDEFGHIJKLMNOPQRSTUVWXYZ');
            print(`+-*×/>=<¬∧∨,.⏨@:; ()[]');
        e̲n̲d̲
    )");
    // Note: symbols " ? _ are prohibited in strings.
    // Symbols | can be used only in digraphs.

    // Check result in memory.
    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);            // START
    EXPECT_EQ(machine->mem_load(start+1), 0x02'01'00);  // 0 1 2
    EXPECT_EQ(machine->mem_load(start+2), 0x05'04'03);  // 3 4 5
    EXPECT_EQ(machine->mem_load(start+3), 0x08'07'06);  // 6 7 8
    EXPECT_EQ(machine->mem_load(start+4), 0x00'ff'09);  // 9 \377
    EXPECT_EQ(machine->mem_load(start+5), 103);         // print
    EXPECT_EQ(machine->mem_load(start+6), 0x0c'0b'0a);  // a b c
    EXPECT_EQ(machine->mem_load(start+7), 0x0f'0e'0d);  // d e f
    EXPECT_EQ(machine->mem_load(start+8), 0x12'11'10);  // g h i
    EXPECT_EQ(machine->mem_load(start+9), 0x15'14'13);  // j k l
    EXPECT_EQ(machine->mem_load(start+10), 0x18'17'16); // m n o
    EXPECT_EQ(machine->mem_load(start+11), 0x1b'1a'19); // p q r
    EXPECT_EQ(machine->mem_load(start+12), 0x1e'1d'1c); // s t u
    EXPECT_EQ(machine->mem_load(start+13), 0x21'20'1f); // v w x
    EXPECT_EQ(machine->mem_load(start+14), 0xff'23'22); // y z \377
    EXPECT_EQ(machine->mem_load(start+15), 103);        // print
    EXPECT_EQ(machine->mem_load(start+16), 0x27'26'25); // A B C
    EXPECT_EQ(machine->mem_load(start+17), 0x2a'29'28); // D E F
    EXPECT_EQ(machine->mem_load(start+18), 0x2d'2c'2b); // G H I
    EXPECT_EQ(machine->mem_load(start+19), 0x30'2f'2e); // J K L
    EXPECT_EQ(machine->mem_load(start+20), 0x33'32'31); // M N O
    EXPECT_EQ(machine->mem_load(start+21), 0x36'35'34); // P Q R
    EXPECT_EQ(machine->mem_load(start+22), 0x39'38'37); // S T U
    EXPECT_EQ(machine->mem_load(start+23), 0x3c'3b'3a); // V W X
    EXPECT_EQ(machine->mem_load(start+24), 0xff'3e'3d); // Y Z \377
    EXPECT_EQ(machine->mem_load(start+25), 103);        // print
    EXPECT_EQ(machine->mem_load(start+26), 0x42'41'40); // + - *
    EXPECT_EQ(machine->mem_load(start+27), 0x46'43'42); // × / >
    EXPECT_EQ(machine->mem_load(start+28), 0x4c'4a'48); // = < ¬
    EXPECT_EQ(machine->mem_load(start+29), 0x57'4e'4d); // ∧ ∨ ,
    EXPECT_EQ(machine->mem_load(start+30), 0x59'59'58); // . ⏨ @
    EXPECT_EQ(machine->mem_load(start+31), 0x5d'5b'5a); // : ; space
    EXPECT_EQ(machine->mem_load(start+32), 0x64'63'62); // ( ) [
    EXPECT_EQ(machine->mem_load(start+33), 0x00'ff'65); // ] \377
    EXPECT_EQ(machine->mem_load(start+34), 103);        // print
    EXPECT_EQ(machine->mem_load(start+35), 97);         // STOP
}

TEST_F(x1_machine, digraph_encoding)
{
    compile(R"(
        b̲e̲g̲i̲n̲
            print(` := ');
            print(` |∧ ');
            print(` |= ');
            print(` |<|> ');
            print(` _> ');
            print(` _= ');
            print(` _< ');
            print(` _¬ ');
            print(` _: ');
        e̲n̲d̲
    )");

    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);            // START
    EXPECT_EQ(machine->mem_load(start+1), 0x5d'5c'5d);  // := ≔
    EXPECT_EQ(machine->mem_load(start+4), 0x5d'45'5d);  // |∧ ↑
    EXPECT_EQ(machine->mem_load(start+7), 0x5d'4b'5d);  // |= ≠
    EXPECT_EQ(machine->mem_load(start+10), 0x67'66'5d); // |< |> ` '
    EXPECT_EQ(machine->mem_load(start+13), 0x5d'47'5d); // _> ≥
    EXPECT_EQ(machine->mem_load(start+16), 0x5d'50'5d); // _= ≡
    EXPECT_EQ(machine->mem_load(start+19), 0x5d'49'5d); // _< ≤
    EXPECT_EQ(machine->mem_load(start+22), 0x5d'4f'5d); // _¬ ⊃
    EXPECT_EQ(machine->mem_load(start+25), 0x5d'44'5d); // _: ÷
    EXPECT_EQ(machine->mem_load(start+28), 97);         // STOP
}

TEST_F(x1_machine, string_quotes)
{
    compile(R"(
        b̲e̲g̲i̲n̲
            print(`This is a `string'');
        e̲n̲d̲
    )");

    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);           // START
    EXPECT_EQ(machine->mem_load(start+1), 0x12'11'38); // T h i
    EXPECT_EQ(machine->mem_load(start+2), 0x12'5d'1c); // s ␣ i
    EXPECT_EQ(machine->mem_load(start+3), 0x0a'5d'1c); // s ␣ a
    EXPECT_EQ(machine->mem_load(start+4), 0x1c'66'5d); // ␣ ` s
    EXPECT_EQ(machine->mem_load(start+5), 0x12'1b'1d); // t r i
    EXPECT_EQ(machine->mem_load(start+6), 0x67'10'17); // n g '
    EXPECT_EQ(machine->mem_load(start+7), 0x00'00'ff); // \377
    EXPECT_EQ(machine->mem_load(start+8), 103);        // print
    EXPECT_EQ(machine->mem_load(start+9), 97);         // STOP
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
            print(123);
            print(-123, 67108863);
            print;
            print(-67108863, 0, -0);
        e̲n̲d̲
    )");
    const std::string expect = R"(123
-123
67108863

-67108863
0
-0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, print_reals)
{
    auto output = compile_and_run(R"(
        b̲e̲g̲i̲n̲
            print(123.456);
            print(-123.456, 1.615850303564⏨616);
            print;
            print(-1.615850303564⏨616, 0.0, -0.0);
            print(1.547173023691⏨-617, -1.547173023691⏨-617);
        e̲n̲d̲
    )");
    const std::string expect = R"(123.456
-123.456
1.615850303564e+616

-1.615850303564e+616
0
-0
1.547173023691e-617
-1.547173023691e-617
)";
    EXPECT_EQ(output, expect);
}
