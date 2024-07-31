#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, c_input_encoding)
{
    // Technically it's a wrong program, as print() accepts only
    // a number on input. But for checking encoding it's all right.
    // We are not going to run it.
    compile(R"(
        _b_e_g_i_n
            print(|<0123456789|>);
        _e_n_d
    )");

    // Check result in memory.
    EXPECT_EQ(machine->mem_load(10144), 96);         // START
    EXPECT_EQ(machine->mem_load(10145), 0x02'01'00); // 0 1 2
    EXPECT_EQ(machine->mem_load(10146), 0x05'04'03); // 3 4 5
    EXPECT_EQ(machine->mem_load(10147), 0x08'07'06); // 6 7 8
    EXPECT_EQ(machine->mem_load(10148), 0x00'ff'09); // 9 \377
    EXPECT_EQ(machine->mem_load(10149), 103);        // print
    EXPECT_EQ(machine->mem_load(10150), 97);         // STOP
}
