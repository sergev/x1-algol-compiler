#include "machine.h"

//
// Reset routine
//
void Processor::reset()
{
    core = {};
    machine.trace_exception("Reset");
}

//
// Execute one instruction, placed at address OT+right_instr_flag.
// Return false to continue the program.
// Return true when the program is done and the processor is stopped.
// Emit exception in case of failure.
//
bool Processor::step()
{
    Word word;

    OT &= BITS(15);
    word = machine.mem_fetch(OT);
    OR = word;

    //TODO: unsigned addr   = OR & BITS(15);
    unsigned opcode = (OR >> 12) & 0370;

    // Show instruction: address, opcode and mnemonics.
    machine.trace_instruction(opcode);

    OT += 1;

    switch (opcode) {
    //TODO: process instructions
    default:
        // Unknown instruction - cannot happen.
        throw Exception("Unknown instruction");
    }

    return false;
}
