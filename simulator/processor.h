#ifndef X1_PROCESSOR_H
#define X1_PROCESSOR_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "virtual_stack.h"
#include "x1_arch.h"

class Machine;

//
// Internal state of the processor.
//
struct CoreState {
    unsigned B; // stack address + sign bit
    Word A;     // accumulator
    Word S;     // extension of accumulator
    bool C;     // condition
    bool L;     // sign bit at the last condition-setting instruction
};

//
// Offsets in stack relative to frame_ptr.
//
namespace Frame_Offset {
enum {
    RESULT  = -1, // Result to be returned
    FP      = 0,  // Caller's frame pointer
    PC      = 1,  // Caller's program counter
    SP      = 2,  // Caller's stack base
    BN      = 3,  // Lexical scope block level (number)
    DISPLAY = 4,  // Previous display[bn]
    ARG     = 5,  // First argument of procedure, must be 5
};
};

//
// When retrieving a formal parameter, apply the following operation.
//
enum class Formal_Op {
    PUSH_VALUE,     // Push number on stack
    PUSH_ADDRESS,   // Push address on stack
    PUSH_STRING,    // Push string on stack
    ADD,            // Add number to top of stack
    SUBTRACT,       // Subtract number from top of stack
    MULTIPLY,       // Multily top of stack by number
    DIVIDE,         // Divide top of stack by number
    REMOVE_ARG,     // Remove dummy argument from stack
};
std::ostream &operator<<(std::ostream &out, const Formal_Op &op);

//
// Electrologica X1 processor.
//
class Processor {
private:
    // Reference to the machine.
    Machine &machine;

    // Current state.
    struct CoreState core {};

    // Previous state, for tracing.
    struct CoreState prev {};
    unsigned prev_frame_ptr{};
    unsigned prev_stack_ptr{};

    unsigned OT{}; // order counter
    unsigned OR{}; // order register

    // Pointer to a procedure frame in stack.
    unsigned frame_ptr{};

    // Base of stack in current block.
    unsigned stack_base{};

    // Target of a non-local goto.
    unsigned goto_frame{};
    bool goto_flag{};

    // Frame pointers for each block level.
    unsigned display[32]{};

    // Stack contents is mapped to this virtual address.
    static unsigned const STACK_BASE = 0100000;

    // Apply this operation when returning from implicit subroutine (on EIS).
    // Indexed by frame pointer.
    std::unordered_map<unsigned, Formal_Op> eis_operation;

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

    // Have we reached given address?
    bool at_address(unsigned addr, unsigned fp) const { return OT == addr && frame_ptr == fp; }

    // Get frame pointer.
    unsigned get_frame_ptr() const { return frame_ptr; }

    // Print trace info.
    void print_instruction();
    void print_registers();

    // Get lexical scope level, or block number (BN).
    unsigned get_block_level() const;
    unsigned get_display(unsigned block_level) const;

    // Update all display[] entries starting from given level and frame.
    void update_display(unsigned block_level, unsigned fp);

    // Roll stack back to the 'goto' frame, starting from base.
    bool roll_back(unsigned frame_base);

private:
    // Invoke run-time routine.
    // Return true when the processor is stopped.
    bool call_opc(unsigned opc);

    // Loads a real value at addr.
    Real load_real(unsigned addr);

    // Create frame in stack for new procedure block.
    void frame_create(unsigned ret_addr, unsigned num_args);

    // Deallocate frame in stack when leaving the procedure.
    // Return address is returned.
    unsigned frame_release();

    // Allocate local variables.
    void allocate_stack(unsigned nwords);

    // Convert dynamic address of variable (relative to stack frame)
    // into offset in stack.
    unsigned address_in_stack(unsigned dynamic_addr);

    // Extract descriptor of actual argument.
    unsigned arg_descriptor(unsigned dynamic_addr);
    unsigned arg_address(unsigned dynamic_addr, unsigned arg_descr);
    void get_arg_display(unsigned const arg, unsigned &block_level, unsigned &prev_display);

    // Store value given by src cell.
    // Write it to memory address given by dest cell.
    void store_value(const Stack_Cell &dest, const Stack_Cell &src);

    // Load value by the address in the src cell.
    Stack_Cell load_value(const Stack_Cell &src);

    // The common part of all OPCs dealing with dynamic integers.
    Stack_Cell get_dynamic_int(unsigned dynamic_addr);

    // The common part of all OPCs dealing with dynamic reals.
    Stack_Cell get_dynamic_real(unsigned dynamic_addr);

    // Retrieve value of formal parameter.
    void take_formal(unsigned dynamic_addr, Formal_Op op);

    // Get address of procedure parameter at dynamic address.
    unsigned get_formal_proc(unsigned dynamic_addr);

    // Set lexical scope level, or block number (BN).
    void set_block_level(unsigned block_level);

    // Update display[n] value.
    void set_display(unsigned block_level, unsigned value);

    // Helper methods for dynamic arrays.
    void make_storage_function_frame(int elt_size);
    void make_value_array_function_frame(int elt_size);
    Word load_word(unsigned addr);

    // Apply given operation to the retrieved address of formal parameter.
    void apply_operation(Formal_Op post_op, unsigned addr, Cell_Type type);
};

//
// Non-local gotos are implemented as exceptions.
//
class Non_Local_Goto {
    // Empty.
};

#endif // X1_PROCESSOR_H
