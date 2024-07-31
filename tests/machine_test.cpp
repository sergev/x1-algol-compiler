#include <fstream>

#include "fixture.h"

TEST_F(x1_machine, trace_two_by_two)
{
#if 0
    // Store the test code.
    store_word(010, x1_asm("xta 2000, arx 2001"));
    store_word(011, x1_asm("stop 12345(6), utc")); // Magic opcode: Pass
    store_word(02000, 0'0000'0000'0000'0013ul);
    store_word(02001, 0'0000'0000'0000'0001ul);

    // Enable trace.
    std::string trace_filename = get_test_name() + ".trace";
    machine->redirect_trace(trace_filename.c_str(), true);

    // Run the code.
    machine->cpu.set_pc(010);
    machine->run();

    // Check registers.
    EXPECT_EQ(machine->cpu.get_pc(), 011u);
    EXPECT_EQ(machine->cpu.get_acc(), 014u);

    // Read the trace.
    auto trace = file_contents_split(trace_filename);

    // Check output.
    static const std::vector<std::string> expect = {
        // clang-format off
        "00010 L: 00 010 2000 xta 2000",
        "      Memory Read [02000] = 0000 0000 0000 0013",
        "      ACC = 0000 0000 0000 0013",
        "      RAU = 04",
        "00010 R: 00 013 2001 arx 2001",
        "      Memory Read [02001] = 0000 0000 0000 0001",
        "      ACC = 0000 0000 0000 0014",
        "      RAU = 10",
        "00011 L: 06 33 12345 stop 12345(6)",
        // clang-format on
    };
    EXPECT_EQ(trace, expect);
#endif
}
