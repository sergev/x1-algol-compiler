#ifndef X1_PROCESSOR_H
#define X1_PROCESSOR_H

#include <cstdint>
#include <string>

#include "x1_arch.h"
#include "virtual_stack.h"

class Machine;

//
// Internal state of the processor.
//
struct CoreState {
    unsigned B;  // stack address + sign bit
    Word A;      // accumulator
    Word S;      // extension of accumulator
    bool C;      // condition
    bool L;      // sign bit at the last condition-setting instruction
};

//
// Offsets in stack relative to frame_ptr.
//
namespace Frame_Offset {
    enum {
        FP = 0,      // Caller's frame pointer
        PC = 1,      // Caller's program counter
        SP = 2,      // Caller's stack base
        RESULT = 3,  // Result to be returned
        BN = 4,      // Lexical scope block level (number)
        DISPLAY = 5, // Previous display[bn]
        ARG = 6,     // First argument of procedure
    };
};

//
// Electrologica X1 processor.
//
class Processor {
private:
    // Reference to the machine.
    Machine &machine;

    // Current state.
    struct CoreState core{};

    // Previous state, for tracing.
    struct CoreState prev{};
    unsigned prev_frame_ptr{};
    unsigned prev_stack_ptr{};

    unsigned OT{}; // order counter
    unsigned OR{}; // order register

    // Pointer to a procedure frame in stack.
    unsigned frame_ptr{};

    // Base of stack in current block.
    unsigned stack_base{};

    // Frame pointers for each block level.
    std::vector<unsigned> display[32];

    // Stack contents is mapped to this virtual address.
    static unsigned const STACK_BASE = 0100000;

public:
    // Stack of arguments for OPC.
    Virtual_Stack stack;

    // Constructor.
    explicit Processor(Machine &mach) : machine(mach) { reset(); }

    // Reset to initial state.
    void reset();

    // Simulate one instruction.
    // Return true when the processor is stopped.
    bool step();

    // Set register value.
    void set_ot(unsigned val) { OT = val; }
    void set_b(unsigned val) { core.B = val; }
    void set_c(bool val) { core.C = val; }
    void set_a(Word val) { core.A = val; }
    void set_s(Word val) { core.S = val; }

    // Get register value.
    unsigned get_ot() const { return OT; }
    unsigned get_b() const { return core.B; }
    bool get_c() const { return core.C; }
    Word get_a() const { return core.A; }
    Word get_s() const { return core.S; }

    // Print trace info.
    void print_instruction();
    void print_registers();

private:
    // Invoke run-time routine.
    // Return true when the processor is stopped.
    bool call_opc(unsigned opc);

    // Loads a real value at addr.
    Real load_real(unsigned addr);

    // Create frame in stack for new procedure block.
    void frame_create(unsigned ret_addr, unsigned num_args, bool need_result);

    // Deallocate frame in stack when leaving the procedure.
    // Return address is returned.
    unsigned frame_release();

    // Allocate local variables.
    void allocate_stack(unsigned nwords);

    // Convert dynamic address of variable (relative to stack frame)
    // into offset in stack.
    unsigned address_in_stack(unsigned dynamic_addr);
    unsigned address_in_caller_stack(unsigned block_level, unsigned offset);

    // Extract descriptor of actual argument.
    unsigned arg_descriptor(unsigned dynamic_addr);

    // Store value given by src cell.
    // Write it to memory address given by dest cell.
    void store_value(const Stack_Cell &dest, const Stack_Cell &src);

    // Load value by the address in the src cell.
    Stack_Cell load_value(const Stack_Cell &src);

    // The common part of all OPCs dealing with dynamic integers.
    Stack_Cell get_dynamic_int(unsigned dynamic_addr);

    // The common part of all OPCs dealing with dynamic reals.
    Stack_Cell get_dynamic_real(unsigned dynamic_addr);

    // Get value at dynamic address and push it on stack.
    void push_formal_value(unsigned dynamic_addr);
    void push_formal_address(unsigned dynamic_addr);

    // Get/set lexical scope level, or block number (BN).
    unsigned get_block_level() const;
    void set_block_level(unsigned block_level);

    // Update display[n] value.
    void push_display(unsigned block_level, unsigned value);
    void pop_display(unsigned block_level);
    unsigned get_display(unsigned block_level) const;

    // Helper methods for dynamic arrays.
    void make_storage_function_frame(int elt_size);
    Word load_word(unsigned addr);
};

#endif // X1_PROCESSOR_H
