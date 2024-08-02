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
    EXPECT_EQ(machine->mem_load(10112), 96);         // START

    EXPECT_EQ(machine->mem_load(10113), 0x02'01'00); // 0 1 2
    EXPECT_EQ(machine->mem_load(10114), 0x05'04'03); // 3 4 5
    EXPECT_EQ(machine->mem_load(10115), 0x08'07'06); // 6 7 8
    EXPECT_EQ(machine->mem_load(10116), 0x00'ff'09); // 9 \377
    EXPECT_EQ(machine->mem_load(10117), 103);        // print

    EXPECT_EQ(machine->mem_load(10118), 0x0c'0b'0a); // a b c
    EXPECT_EQ(machine->mem_load(10119), 0x0f'0e'0d); // d e f
    EXPECT_EQ(machine->mem_load(10120), 0x12'11'10); // g h i
    EXPECT_EQ(machine->mem_load(10121), 0x15'14'13); // j k l
    EXPECT_EQ(machine->mem_load(10122), 0x18'17'16); // m n o
    EXPECT_EQ(machine->mem_load(10123), 0x1b'1a'19); // p q r
    EXPECT_EQ(machine->mem_load(10124), 0x1e'1d'1c); // s t u
    EXPECT_EQ(machine->mem_load(10125), 0x21'20'1f); // v w x
    EXPECT_EQ(machine->mem_load(10126), 0xff'23'22); // y z \377
    EXPECT_EQ(machine->mem_load(10127), 103);        // print

    EXPECT_EQ(machine->mem_load(10128), 0x27'26'25); // A B C
    EXPECT_EQ(machine->mem_load(10129), 0x2a'29'28); // D E F
    EXPECT_EQ(machine->mem_load(10130), 0x2d'2c'2b); // G H I
    EXPECT_EQ(machine->mem_load(10131), 0x30'2f'2e); // J K L
    EXPECT_EQ(machine->mem_load(10132), 0x33'32'31); // M N O
    EXPECT_EQ(machine->mem_load(10133), 0x36'35'34); // P Q R
    EXPECT_EQ(machine->mem_load(10134), 0x39'38'37); // S T U
    EXPECT_EQ(machine->mem_load(10135), 0x3c'3b'3a); // V W X
    EXPECT_EQ(machine->mem_load(10136), 0xff'3e'3d); // Y Z \377
    EXPECT_EQ(machine->mem_load(10137), 103);        // print

    EXPECT_EQ(machine->mem_load(10138), 0x42'41'40); // + - *
    EXPECT_EQ(machine->mem_load(10139), 0x46'43'42); // × / >
    EXPECT_EQ(machine->mem_load(10140), 0x4c'4a'48); // = < ¬
    EXPECT_EQ(machine->mem_load(10141), 0x57'4e'4d); // ∧ ∨ ,
    EXPECT_EQ(machine->mem_load(10142), 0x59'59'58); // . ⏨ @
    EXPECT_EQ(machine->mem_load(10143), 0x5d'5b'5a); // : ; space
    EXPECT_EQ(machine->mem_load(10144), 0x64'63'62); // ( ) [
    EXPECT_EQ(machine->mem_load(10145), 0x00'ff'65); // ] \377
    EXPECT_EQ(machine->mem_load(10146), 103);        // print

    EXPECT_EQ(machine->mem_load(10147), 97);         // STOP
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

    EXPECT_EQ(machine->mem_load(10112), 96);         // START
    EXPECT_EQ(machine->mem_load(10113), 0x5d'5c'5d); // := ≔
    EXPECT_EQ(machine->mem_load(10116), 0x5d'45'5d); // |∧ ↑
    EXPECT_EQ(machine->mem_load(10119), 0x5d'4b'5d); // |= ≠
    EXPECT_EQ(machine->mem_load(10122), 0x5d'47'5d); // _> ≥
    EXPECT_EQ(machine->mem_load(10125), 0x5d'50'5d); // _= ≡
    EXPECT_EQ(machine->mem_load(10128), 0x5d'49'5d); // _< ≤
    EXPECT_EQ(machine->mem_load(10131), 0x5d'4f'5d); // _¬ ⊐
    EXPECT_EQ(machine->mem_load(10134), 0x5d'44'5d); // _: ÷
    EXPECT_EQ(machine->mem_load(10137), 97);         // STOP
}

TEST_F(x1_machine, DISABLED_print_integers)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            print(123);
            print(-123);
            print(67108863);
            print(-67108863);
            print(0);
            print(-0);
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

TEST_F(x1_machine, DISABLED_man_or_boy)
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
}
