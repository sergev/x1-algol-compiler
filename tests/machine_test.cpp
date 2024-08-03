#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, input_encoding)
{
    // Technically it's a wrong program, as print() accepts only
    // a number on input. But for checking encoding it's all right.
    // We are not going to run it.
    compile(R"(
        _b_e_g_i_n
            print(|<0123456789|>);
            print(|<abcdefghijklmnopqrstuvwxyz|>);
            print(|<ABCDEFGHIJKLMNOPQRSTUVWXYZ|>);
            print(|<+-*×/>=<¬∧∨,.⏨@:; ()[]|||||>);
        _e_n_d
    )");
    // Note: symbols ' " ? _ are prohibited in strings.
    // Repeated symbols | are ignored.

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
        _b_e_g_i_n
            print(|< := |>);
            print(|< |∧ |>);
            print(|< |= |>);
            print(|< _> |>);
            print(|< _= |>);
            print(|< _< |>);
            print(|< _¬ |>);
            print(|< _: |>);
        _e_n_d
    )");

    unsigned start = machine->get_entry(0);
    EXPECT_EQ(machine->mem_load(start), 96);            // START
    EXPECT_EQ(machine->mem_load(start+1), 0x5d'5c'5d);  // := ≔
    EXPECT_EQ(machine->mem_load(start+4), 0x5d'45'5d);  // |∧ ↑
    EXPECT_EQ(machine->mem_load(start+7), 0x5d'4b'5d);  // |= ≠
    EXPECT_EQ(machine->mem_load(start+10), 0x5d'47'5d); // _> ≥
    EXPECT_EQ(machine->mem_load(start+13), 0x5d'50'5d); // _= ≡
    EXPECT_EQ(machine->mem_load(start+16), 0x5d'49'5d); // _< ≤
    EXPECT_EQ(machine->mem_load(start+19), 0x5d'4f'5d); // _¬ ⊃
    EXPECT_EQ(machine->mem_load(start+22), 0x5d'44'5d); // _: ÷
    EXPECT_EQ(machine->mem_load(start+25), 97);         // STOP
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

    EXPECT_TRUE(stack.back().is_real_addr());
    EXPECT_EQ(stack.back().get_addr(), 23456);
    stack.pop();
    EXPECT_EQ(stack.count(), 3);

    EXPECT_TRUE(stack.back().is_real_value());
    EXPECT_EQ(stack.back().get_real(), 012'37'64007'36'67'22743ull);
    stack.pop();
    EXPECT_EQ(stack.count(), 2);

    EXPECT_TRUE(stack.back().is_int_addr());
    EXPECT_EQ(stack.back().get_addr(), 12345);
    stack.pop();
    EXPECT_EQ(stack.count(), 1);

    EXPECT_TRUE(stack.back().is_int_value());
    EXPECT_EQ(stack.back().get_int(), 12345678);
    stack.pop();
    EXPECT_EQ(stack.count(), 0);
}

TEST_F(x1_machine, print_integers)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(123);
            print(-123, 67108863);
            print;
            print(-67108863, 0, -0);
        _e_n_d
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
        _b_e_g_i_n
            print(123.456);
            print(-123.456, 1.615850303565⏨616);
            print;
            print(-1.615850303565⏨616, 0.0, -0.0);
            print(1.547173023691⏨-617, -1.547173023691⏨-617);
        _e_n_d
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

TEST_F(x1_machine, function_abs)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(abs(-123.456));
            print(abs(123.456));
            print(abs(-1.615850303565⏨616));
            print(abs(1.615850303565⏨616));
            print(abs(-0.0));
            print(abs(0.0));
            print(abs(-67108863));
            print(abs(67108863));
            print(abs(-0));
            print(abs(0));
        _e_n_d
    )");
    const std::string expect = R"(123.456
123.456
1.615850303564e+616
1.615850303564e+616
0
0
67108863
67108863
0
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sign)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(sign(-123.456));
            print(sign(123.456));
            print(sign(-1.615850303565⏨616));
            print(sign(1.615850303565⏨616));
            print(sign(-0.0));
            print(sign(0.0));
            print(sign(-1));
            print(sign(1));
            print(sign(-67108863));
            print(sign(67108863));
            print(sign(-0));
            print(sign(0));
        _e_n_d
    )");
    const std::string expect = R"(-1
1
-1
1
0
0
-1
1
-1
1
0
0
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sqrt)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(sqrt(2));
            print(sqrt(99999));
            print(sqrt(1.01));
            print(sqrt(0.99));
            print(sqrt(0.0003));
        _e_n_d
    )");
    const std::string expect = R"(1.414213562373
316.2261848738
1.004987562112
0.9949874371068
0.0173205080757
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_sin)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(sin(0.0001));
            print(sin(-0.0001));
            print(sin(0.7854));
            print(sin(-0.7854));
            print(sin(1.5));
            print(sin(-1.6));
            print(sin(3141.6));
            print(sin(-3141.6));
        _e_n_d
    )");
    const std::string expect = R"(9.999999983334e-05
-9.999999983334e-05
0.7071080798596
-0.7071080798596
0.9974949866037
-0.9995736030414
0.007346345616369
-0.007346345616369
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_cos)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(cos(0.01));
            print(cos(-0.01));
            print(cos(0.7854));
            print(cos(-0.7854));
            print(cos(1.5));
            print(cos(-1.6));
            print(cos(3141.6));
            print(cos(-3141.6));
        _e_n_d
    )");
    const std::string expect = R"(0.9999500004169
0.9999500004169
0.707105482511
0.707105482511
0.07073720166773
-0.02919952230164
0.9999730152385
0.9999730152385
)";
    EXPECT_EQ(output, expect);
}

//
// Cannot compile this program: it wants a library tape.
// "Need MCP 3"
// "Need MCP 4"
//
TEST_F(x1_machine, DISABLED_function_arctan)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(arctan(0.0001));
            print(arctan(-0.0001));
            print(arctan(0.5));
            print(arctan(-0.5));
            print(arctan(2));
            print(arctan(-2));
            print(arctan(999.9));
            print(arctan(-999.9));
        _e_n_d
    )");
    const std::string expect = R"(0.000100
-0.000100
0.463648
-0.463648
1.107149
-1.107149
1.569796
-1.569796
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_ln)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(ln(0.0001));
            print(ln(0.5));
            print(ln(0.9999));
            print(ln(1.0001));
            print(ln(2));
            print(ln(4));
            print(ln(8));
            print(ln(99999));
        _e_n_d
    )");
    const std::string expect = R"(-9.210340371981
-0.6931471805601
-0.0001000050005356
9.999499962621e-05
0.6931471805601
1.38629436112
2.079441541679
11.51291546492
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, function_exp)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(exp(-6));
            print(exp(-1));
            print(exp(0.0001));
            print(exp(1));
            print(exp(6));
        _e_n_d
    )");
    const std::string expect = R"(0.002478752176668
0.3678794411712
1.000100005
2.71828182846
403.4287934927
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, man_or_boy)
{
    // Only compile for now.
    compile(R"(
        _b_e_g_i_n
            _r_e_a_l _p_r_o_c_e_d_u_r_e A(k, x1, x2, x3, x4, x5);
            _v_a_l_u_e k; _i_n_t_e_g_e_r k;
            _r_e_a_l x1, x2, x3, x4, x5;
            _b_e_g_i_n
                _r_e_a_l _p_r_o_c_e_d_u_r_e B;
                _b_e_g_i_n
                    k := k - 1;
                    B := A := A(k, B, x1, x2, x3, x4);
                _e_n_d;
                A := _i_f k < 1 _t_h_e_n x4 + x5 _e_l_s_e B;
            _e_n_d;
            print(A(10, 1.0, -1.0, -1.0, 1.0, 0.0));
        _e_n_d
    )");

    // Check symbol table.
    EXPECT_EQ(machine->get_symbol("A"), 2);
    EXPECT_EQ(machine->get_symbol("B"), 10);

    // Check entry address.
    EXPECT_EQ(machine->get_entry(0), 10048);

    //TODO: run and check result
}
