#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, procedure_arg0)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _p_r_o_c_e_d_u_r_e print123();
            _b_e_g_i_n
                print(123);
            _e_n_d;
            print123;
        _e_n_d
    )");
    const std::string expect = "123\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_integer_by_value)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _p_r_o_c_e_d_u_r_e print int(num);
                _v_a_l_u_e num;
                _i_n_t_e_g_e_r num;
            _b_e_g_i_n
                print(num);
            _e_n_d;
            print int(123);
            print int(-123);
            print int(67108863);
            print int(-67108863);
            print int(0);
            print int(-0);
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

TEST_F(x1_machine, arg1_integer_by_name)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _i_n_t_e_g_e_r i;
            _p_r_o_c_e_d_u_r_e get int(num);
                _i_n_t_e_g_e_r num;
            _b_e_g_i_n
                num := -123;
            _e_n_d;
            get int(i);
            print(i);
        _e_n_d
    )");
    const std::string expect = "-123\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, arg1_real_by_value)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _p_r_o_c_e_d_u_r_e print real(num);
                _v_a_l_u_e num;
                _r_e_a_l num;
            _b_e_g_i_n
                print(num);
            _e_n_d;
            print real(123.456);
            print real(-123.456);
            print real(1.615850303564⏨616);
            print real(-1.615850303564⏨616);
            print real(0.0);
            print real(-0.0);
            print real(1.547173023691⏨-617);
            print real(-1.547173023691⏨-617);
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

TEST_F(x1_machine, arg1_real_by_name)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _r_e_a_l i;
            _p_r_o_c_e_d_u_r_e get real(num);
                _r_e_a_l num;
            _b_e_g_i_n
                num := -123.456;
            _e_n_d;
            get real(i);
            print(i);
        _e_n_d
    )");
    const std::string expect = "-123.456\n";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, return_integer)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _i_n_t_e_g_e_r _p_r_o_c_e_d_u_r_e div pi(num);
                _v_a_l_u_e num;
                _r_e_a_l num;
            _b_e_g_i_n
                div pi := entier(num / 3.14159265359);
            _e_n_d;
            print(div pi(9.99));
            print(div pi(-15.5));
            print(div pi(30.7));
        _e_n_d
    )");
    const std::string expect = R"(3
-5
9
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, return_real)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _r_e_a_l _p_r_o_c_e_d_u_r_e mul pi(num);
                _v_a_l_u_e num;
                _i_n_t_e_g_e_r num;
            _b_e_g_i_n
                mul pi := num × 3.14159265359;
            _e_n_d;
            print(mul pi(2));
            print(mul pi(-3));
            print(mul pi(4));
        _e_n_d
    )");
    const std::string expect = R"(6.283185307177
-9.424777960769
12.56637061435
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, real_args)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _p_r_o_c_e_d_u_r_e args(a, b, c, d, e);
                _v_a_l_u_e a, b, c, d, e;
                _r_e_a_l a, b, c, d, e;
            _b_e_g_i_n
                print(a, b, c, d, e);
            _e_n_d;
            args(123.456, 1.615850303564⏨616, 0.0,
                 67108863, 1.547173023691⏨-617);
        _e_n_d
    )");
    const std::string expect = R"(123.456
1.615850303564e+616
0
67108863
1.547173023691e-617
)";
    EXPECT_EQ(output, expect);
}

TEST_F(x1_machine, DISABLED_integer_factorial)
{
    auto output = compile_and_run(R"(
        _b_e_g_i_n
            _i_n_t_e_g_e_r _p_r_o_c_e_d_u_r_e fact(num);
                _v_a_l_u_e num;
                _i_n_t_e_g_e_r num;
            _b_e_g_i_n
                fact := _i_f num < 2 _t_h_e_n 1 _e_l_s_e num × fact(num - 1);
            _e_n_d;
            print(fact(3));
        _e_n_d
    )");
    const std::string expect = R"(6
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
