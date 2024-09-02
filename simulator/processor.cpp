#include <algorithm>
#include <cmath>
#include <iostream>

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
// Only a subset of X1 instructions is required for Algol compiler:
//   0A 0   A       A += addr
//   0A 0 X0        A += M[addr]
//   0A 0 X0 B      A += M[addr + B]
//   0A 0 X0 C      A += M[addr + B], addr := A
//   2A 0   A       A := addr
//   2B 0   A       B := addr
//   2B 0 X0        B := M[addr]
//   2S 0   A       S := addr
//   2S 0 X0        S := M[addr]
//   2T 0   A       T := addr
//   2T 0 X0        T := M[addr]
//   4A 0 X0        M[addr] += A
// N 2T 0 X0        if (!C) T := M[addr]
// Y 2A 0   A       if (C) A := addr
// Y 2A 0 X0  P     if (C) A := M[addr], C := (A > +0)
// Y 6A 0   A       if (C) M[addr] := A
//
bool Processor::step()
{
    bool done_flag{};

    OT &= BITS(15);
    OR = machine.mem_fetch(OT);

    unsigned opcode = (OR >> 15) & BITS(12);
    unsigned addr   = OR & BITS(15);

    // Show instruction: address, opcode and mnemonics.
    machine.trace_instruction(opcode);

    // Proceed to next instruction by default.
    OT += 1;

    // Format of X1 instructions:
    //
    //              0- A += M[n]        T += 1 + M[n]
    //              1- A -= M[n]        T += 1 - M[n]
    //              2- A := M[n]        T := M[n]
    //              3- A := - M[n]      T := - M[n]
    //   0-A        4- M[n] += A        M[m] -= 1; T := n
    //   1-S        5- M[n] -= A
    //   4-B        6- M[n] := A        M[m+8] := T + 1; T := n
    //   5-T        7- M[n] := -A
    // function  function
    //  letter    digit
    //      \    /
    //       \  /
    //        42 20 23627  B := addr
    //          /| \.
    //         / |  \.
    // address/  |   \condition
    //  mode     |    reaction
    //  2-A      |      U-1
    //  4-B      |      Y-2
    //  6-C      |      N-3
    //       condition
    //        setting
    //          P-04
    //          Z-10
    //          E-14

    // Y and N conditions reactions has the same meaning for all instructions
    /// thus it can be checked globally.
    switch (opcode & 3) {
    case 0: // regular
    case 1: // Undisturbed, unconditional
        break;
    case 2: // Yes condition
        if (!core.C)
            return done_flag;
        opcode &= ~3;
        break;
    case 3: // No condition
        if (core.C)
            return done_flag;
        opcode &= ~3;
        break;
    }
    switch (opcode) {
    case 0:
        done_flag = call_opc(addr);
        break;

    case 002'20:
        core.A = addr;
        break;

    case 002'44:
        // Read integer value from the topmost stack cell.
        if (addr == 077774) {
            // 2A 32764 X0 B P
            core.A = stack.top().get_int();
            // Set the condition to "A has positive (0) sign bit".
            core.C = (core.A & ONEBIT(26)) ? 0 : 1;
            // Save the sign bit.
            core.L = core.A & ONEBIT(26);
        } else
            goto unknown;
        break;

    case 003'20:
        core.A = ~addr & BITS(27);
        break;

    case 003'50:
        if (addr == 077777) {
            // 3A 32767 X0 B Z
            // Reading inverted cell type of the topmost stack cell.
            switch (stack.top().type) {
            case Cell_Type::INTEGER_VALUE:
                core.A = 0;
                break;
            case Cell_Type::INTEGER_ADDRESS:
                core.A = -12345; // some negative value
                break;
            case Cell_Type::REAL_VALUE:
                core.A = 1;
                break;
            case Cell_Type::REAL_ADDRESS:
                core.A = -23456; // some negative value
                break;
            default:
                throw std::runtime_error("Bad stack cell type");
            }
            // Setting the condition to A == 0
            core.C = (core.A == 0);
            core.L = core.A & ONEBIT(26);
        } else
            goto unknown;
        break;

    case 004'00:
        switch (addr) {
        case 061:
            // Allocate local variables.
            // Literally the instruction means: M[061] += A.
            // But we have to operate on our virtual stack instead.
            allocate_stack(core.A);
            break;
        case 062:
            // Increase stack pointer by amount in register A.
            stack_base += core.A;
            break;
        default:
            goto unknown;
        }
        break;

    case 006'00:
        machine.mem_store(addr, core.A);
        break;

    case 006'40:
        if (addr == 077774) {
            // 6A 32764 X0 B
            // Patching (integer) value of the topmost stack cell
            stack.top().value = core.A;
        } else
            goto unknown;
        break;

    case 012'00:
        core.S = machine.mem_load(addr);
        break;

    case 012'20:
        core.S = addr;
        break;

    case 012'40:
        core.S = machine.mem_load((addr + core.B) & BITS(15));
        break;

    case 016'00:
        machine.mem_store(addr, core.S);
        break;

    case 034'31:
        // U 0LS addr A Z
        core.C = core.S == addr;
        break;

    case 036'20:
        // 2LS addr A
        core.S &= addr;
        break;

    case 040'20:
        core.B += addr;
        break;

    case 042'00:
        core.B = machine.mem_load(addr) % 32768;
        break;

    case 042'20:
        core.B = addr;
        break;

    case 042'40:
        if (addr == 5) {
            // This instruction is used by PRINTTEXT to find out
            // the value of its formal parameter.
            formal_mode = Formal_Op::PUSH_STRING;
            push_formal_value(0241);
            formal_mode = Formal_Op::PUSH_VALUE;
            core.B = stack.pop_addr();
        } else
            core.B = machine.mem_load((addr + core.B) & BITS(15));
        break;

    case 046'00:
        machine.mem_store(addr, core.B);
        break;

    case 051'00:
        OT -= 1 + machine.mem_load(addr);
        break;

    case 052'20:
        OT = addr;
        break;

    case 054'04: {
        // 4T addr 0 P
        unsigned w = machine.mem_load(0);
        machine.mem_store(0, (w + 0777777776 + (w > 1)) & BITS(27));
        if (w > 1) { // therefore the decremented value is > 0
            OT = addr;
        }
        break;
    }
    case 067'00:
        if (addr != 040000)
            goto unknown;
        throw std::runtime_error("Standard function operand type check failed");

    case 067'14: {
        // Round shift to the right, 1P amount rr E
        unsigned amount = addr & 037;
        switch (addr >> 5) {
        case 0: // AA
            core.A = (core.A >> amount | core.A << (27 - amount)) & BITS(27);
            break;
        case 1: // SS
            core.S = (core.S >> amount | core.S << (27 - amount)) & BITS(27);
            break;
        default:
            goto unknown;
        }
        switch (addr >> 5) {
        case 0:
            core.C = ((core.A & ONEBIT(26)) == ONEBIT(26)) == core.L;
            core.L = core.A & ONEBIT(26);
            break;
        case 1:
            core.C = ((core.S & ONEBIT(26)) == ONEBIT(26)) == core.L;
            core.L = core.S & ONEBIT(26);
            break;
        }
    }

    case 077'00: {
        // Clear shift to the right, 3P amount rr
        unsigned amount = addr & 037;
        switch (addr >> 5) {
        case 0: // AA
            core.A = (core.A >> amount | -(core.A >> 26) << (27 - amount)) & BITS(27);
            break;
        case 1: // SS
            core.S = (core.S >> amount | -(core.S >> 26) << (27 - amount)) & BITS(27);
            break;
        default:
            goto unknown;
        }
        break;
    }

        // TODO: process other instructions

    default:
    unknown:
        // Unknown instruction - cannot happen.
        throw std::runtime_error("Unknown instruction " + to_octal(OR));
    }

    return done_flag;
}

//
// Read two words from memory at addr.
// Return them as Real value.
//
Real Processor::load_real(unsigned addr)
{
    if (addr < STACK_BASE) {
        // From memory.
        Word hi = machine.mem_load(addr);
        Word lo = machine.mem_load(addr + 1);
        return x1_words_to_real(hi, lo);
    } else {
        // From stack.
        return stack.get(addr - STACK_BASE).get_real();
    }
}

Stack_Cell Processor::load_value(const Stack_Cell &src)
{
    auto addr = src.get_addr();
    if (src.is_int_addr()) {
        Word result = load_word(addr);
        return Stack_Cell{ Cell_Type::INTEGER_VALUE, result };
    } else if (src.is_real_addr()) {
        Real result = load_real(addr);
        return Stack_Cell{ Cell_Type::REAL_VALUE, result };
    } else {
        throw std::runtime_error("load_value() invoked on a non-address operand");
    }
}

Stack_Cell Processor::get_dynamic_int(unsigned dynamic_addr)
{
    unsigned addr = address_in_stack(dynamic_addr);
    auto result   = stack.get(addr);
    if (!result.is_int_value()) {
        throw std::runtime_error("Unexpected type instead of dynamic integer");
    }
    return result;
}

Stack_Cell Processor::get_dynamic_real(unsigned dynamic_addr)
{
    unsigned addr = address_in_stack(dynamic_addr);
    auto result   = stack.get(addr);
    if (!result.is_real_value()) {
        throw std::runtime_error("Unexpected type instead of dynamic real");
    }
    return result;
}

//
// Create frame in stack for new procedure block.
//
void Processor::frame_create(unsigned ret_addr, unsigned num_args)
{
    auto new_frame_ptr = stack.count();

    stack.push_int_addr(frame_ptr);  // offset 0: previos frame pointer
    stack.push_int_addr(ret_addr);   // offset 1: return address
    stack.push_int_addr(stack_base); // offset 2: base of the stack
    stack.push_int_addr(0);          // offset 3: place for block level
    stack.push_int_addr(0);          // offset 4: place for saved display

    // For each parameter, store info about parent display (PARD).
    // "The second word contains the FP value and the block number belonging to
    // the block in which the corresponding procedure statement is given." (E.W.Dijkstra)
    //
    auto parent_level   = get_block_level();
    auto parent_display = parent_level << 15 | frame_ptr;

    for (unsigned i = 0; i < num_args; i++) {
        // Allocate formal parameters: two cells per parameter.
        stack.push_int_value(0);
        stack.push_int_addr(parent_display);
    }
    frame_ptr  = new_frame_ptr;
    stack_base = stack.count();
}

//
// Deallocate frame in stack when leaving the procedure.
// Return address is returned.
//
unsigned Processor::frame_release()
{
    if (frame_ptr >= stack.count()) {
        throw std::runtime_error("No frame stack to release");
    }
    auto new_stack_ptr = frame_ptr;
    auto ret_addr      = stack.get(frame_ptr + Frame_Offset::PC).get_addr();
    stack_base         = stack.get(frame_ptr + Frame_Offset::SP).get_addr();
    frame_ptr          = stack.get(frame_ptr + Frame_Offset::FP).get_addr();
    stack.erase(new_stack_ptr);

    // Update display[BN...0] by unwinding stack.
    update_display(get_block_level());

    return ret_addr;
}

//
// Allocate local variables.
//
void Processor::allocate_stack(unsigned nwords)
{
    for (unsigned i = 0; i < nwords; i++) {
        // Allocate local variables.
        stack.push_int_value(0);
    }
}

//
// Roll stack back to the 'goto' frame, starting from base.
// Return true on success.
// Return false when the base is reached.
//
bool Processor::roll_back(unsigned frame_base)
{
    if (goto_frame < frame_base) {
        return false;
    }
    unsigned fp = frame_ptr;
    while (fp >= frame_base) {
        unsigned prev_fp = stack.get(fp + Frame_Offset::FP).get_addr();
        if (prev_fp == goto_frame) {
            frame_ptr = fp;
            frame_release();
            stack.pop();
            return true;
        }
        fp = prev_fp;
    }
    return false;
}

//
// Convert dynamic address of variable (relative to stack frame)
// into static address.
//
// Split dynamic address into block level and memory offset.
// For example, S=0241=161 means 5*32+1.
// Here 1 is the block level, and 5 is offset.
// Get frame pointer of required block from display[block_level].
// Add offset.
//
unsigned Processor::address_in_stack(unsigned dynamic_addr)
{
    auto const block_level = dynamic_addr % 32;
    auto const offset      = (dynamic_addr / 32);

    return get_display(block_level) + offset;
}

//
// Convert dynamic address into static address.
// Assume caller's context, as if restoring display[] for a moment.
//
unsigned Processor::arg_address(unsigned dynamic_addr, unsigned arg_descr)
{
    // Split argument descriptor into lexical level and offset in frame.
    auto const arg_level  = arg_descr >> 22;
    auto const arg_offset = arg_descr & BITS(15);

    // Get caller level and FP from formal descriptor.
    unsigned level, fp;
    get_arg_display(dynamic_addr, level, fp);
    for (; level > 0; level--) {
        if (arg_level == level) {
            return fp + arg_offset;
        }
        fp = stack.get(fp + Frame_Offset::DISPLAY).get_addr();
    }
    throw std::runtime_error("Cannot find arg address");
}

//
// Convert dynamic address of formal parameter into static address
// of actual argument.
//
// Split dynamic address it into block level and memory offset.
// For example, S=0241=161 means 5*32+1.
// Here 1 is the block level, and 5 is offset.
// Get frame pointer of required block from display[block_level].
// Get return address from it, add offset (5) and subtract 8.
// Read word from memory at this address - it contains address
// of actual argument passed to the procedure,
// or address of implicit subroutine.
//
unsigned Processor::arg_descriptor(unsigned dynamic_addr)
{
    auto const arg_num     = ((dynamic_addr / 32) - 5) / 2;
    auto const block_level = dynamic_addr % 32;
    auto const ret_addr    = stack.get(get_display(block_level) + Frame_Offset::PC).get_addr();
    auto const arg_descr   = machine.mem_load(ret_addr - arg_num - 3);

    return arg_descr;
}

void Processor::get_arg_display(unsigned const dynamic_addr, unsigned &block_level,
                                unsigned &prev_display)
{
    auto const addr           = address_in_stack(dynamic_addr);
    auto const formal_display = stack.get(addr + 1).get_int();

    block_level  = formal_display >> 15;
    prev_display = formal_display & BITS(15);
}

//
// Store value given by src cell.
// Write it to stack offset given by dest cell.
// Convert integer value to real when needed.
//
void Processor::store_value(const Stack_Cell &dest, const Stack_Cell &src)
{
    unsigned addr = dest.get_addr();

    switch (dest.type) {
    case Cell_Type::INTEGER_ADDRESS: {
        Word result;
        if (src.is_int_value()) {
            result = src.get_int();
        } else if (src.is_real_value()) {
            result = integer_to_x1((int)roundl(x1_to_ieee(src.get_real())));
        } else {
            throw std::runtime_error("Cannot store address");
        }
        if (addr < STACK_BASE) {
            // Store to memory.
            machine.mem_store(addr, result);
        } else {
            // Store to stack.
            auto &item = stack.get(addr - STACK_BASE);
            item.value = result;
            item.type  = Cell_Type::INTEGER_VALUE;
            Machine::trace_stack(addr - STACK_BASE, item.to_string(), "Write");
        }
        break;
    }
    case Cell_Type::REAL_ADDRESS: {
        Real result;
        if (src.is_real_value()) {
            result = src.get_real();
        } else if (src.is_int_value()) {
            // Convert integer to real.
            Word value = src.get_int();
            if (value == BITS(27)) {
                // Minus zero.
                result = BITS(54);
            } else {
                result = ieee_to_x1((long double)x1_to_integer(value));
            }
        } else {
            throw std::runtime_error("Cannot store address");
        }
        if (addr < STACK_BASE) {
            // Store to memory.
            machine.mem_store(addr, (result >> 27) & BITS(27));
            machine.mem_store(addr + 1, result & BITS(27));
        } else {
            // Store to stack.
            auto &item = stack.get(addr - STACK_BASE);
            item.value = result;
            item.type  = Cell_Type::REAL_VALUE;
            Machine::trace_stack(addr - STACK_BASE, item.to_string(), "Write");
        }
        break;
    }
    default:
        throw std::runtime_error("Bad destination");
    }
}

//
// Read word from memory at dynamic address - it contains descriptor
// of actual argument. It can hold either address of value, or address
// of implicit subroutine. Call it to obtain actual argument value.
//
// Depending on formal_mode, instead of value, return its address.
//
unsigned Processor::get_formal_proc(unsigned dynamic_addr)
{
    unsigned const arg_descr = arg_descriptor(dynamic_addr);
    unsigned const arg_addr  = arg_descr & BITS(15);
    switch (arg_descr >> 15 & 077) {
    case 040:
        return arg_addr;
    default:
        throw std::runtime_error("Unknown descriptor of formal procedure: " + to_octal(arg_descr));
    }
}

//
// Read word from memory at dynamic address - it contains descriptor
// of actual argument. It can hold either address of value, or address
// of implicit subroutine. Call it to obtain actual argument value.
//
// When formal_mode == Formal_Op::PUSH_ADDRESS, instead of value, return its address.
//
void Processor::push_formal_value(unsigned dynamic_addr)
{
    unsigned const arg_descr = arg_descriptor(dynamic_addr);
    unsigned const arg_addr  = arg_descr & BITS(15);
    switch (arg_descr >> 15 & 077) {
    case 000: {
        if (formal_mode != Formal_Op::PUSH_VALUE) {
            // Get real address.
            stack.push_real_addr(arg_addr);
        } else {
            // Get real value from memory.
            stack.push_real_value(load_real(arg_addr));
        }
        break;
    }
    case 020: {
        if (formal_mode != Formal_Op::PUSH_VALUE) {
            // Get integer address.
            stack.push_int_addr(arg_addr);
        } else {
            // Get integer value from memory.
            Word value = machine.mem_load(arg_addr);
            stack.push_int_value(value);
        }
        break;
    }
    case 040: {
        if (formal_mode == Formal_Op::PUSH_STRING) {
            stack.push_int_addr(arg_addr);
            break;
        }
        // Call implicit subroutine.
        // Need to restore previous display[] first.
        unsigned this_frame = frame_ptr;
        unsigned arg_level, arg_frame;
        get_arg_display(dynamic_addr, arg_level, arg_frame);
        machine.trace_level(arg_level, arg_frame);

        // Invoke implicit subroutine in caller's context.
        stack.push_int_value(0); // place for result
        frame_create(OT, 0);
        if (arg_level > 0) {
            // Use caller's frame.
            frame_ptr = arg_frame;
            update_display(arg_level);
        }
        machine.run(arg_addr, OT, this_frame);
        machine.trace_level();
        break;
    }
    case 002: {
        auto const addr = arg_address(dynamic_addr, arg_descr);
        if (formal_mode != Formal_Op::PUSH_VALUE) {
            // Get real address on stack.
            stack.push_real_addr(addr + STACK_BASE);
        } else {
            // Get real value from stack.
            auto const value = stack.get(addr).get_real();
            stack.push_real_value(value);
        }
        break;
    }
    case 022: {
        auto const addr = arg_address(dynamic_addr, arg_descr);
        if (formal_mode != Formal_Op::PUSH_VALUE) {
            // Get integer address on stack.
            stack.push_int_addr(addr + STACK_BASE);
        } else {
            // Get integer value from stack.
            auto const value = stack.get(addr).get_int();
            stack.push_int_value(value);
        }
        break;
    }
    case 006: {
        // Indirection: argument point to another argument in parent's frame.
        unsigned this_frame = frame_ptr;
        unsigned arg_level, arg_frame;
        get_arg_display(dynamic_addr, arg_level, arg_frame);
        machine.trace_level(arg_level, arg_frame);

        frame_ptr = arg_frame;
        update_display(arg_level);
        frame_ptr = this_frame;

        push_formal_value((arg_descr >> 22) | (arg_addr << 5));

        update_display(get_block_level());
        machine.trace_level();
        break;
    }
    default:
        throw std::runtime_error("Unknown descriptor of formal argument: " + to_octal(arg_descr));
    }
}

//
// Get lexical scope level, or block number (BN) from stack.
//
unsigned Processor::get_block_level() const
{
    if (stack_base == 0) {
        // At global level: no frame in stack yet.
        return 0;
    }
    return stack.get(frame_ptr + Frame_Offset::BN).get_addr();
}

//
// Set lexical scope level for this frame.
//
void Processor::set_block_level(unsigned block_level)
{
    auto &bn   = stack.get(frame_ptr + Frame_Offset::BN);
    auto &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);

    if (block_level == 0) {
        throw std::runtime_error("Bad set_block_level");
    }

    // Store block level in stack frame.
    if (bn.value == 0) {
        bn.value = block_level;
        Machine::trace_stack(frame_ptr + Frame_Offset::BN, bn.to_string(), "Write");
    }

    // Save display of previous lexical level.
    if (block_level > 1) {
        disp.value = get_display(block_level - 1);
        Machine::trace_stack(frame_ptr + Frame_Offset::DISPLAY, disp.to_string(), "Write");
    }
}

//
// Update display[n] value.
//
void Processor::set_display(unsigned block_level, unsigned value)
{
    if (block_level == 0) {
        // Global level: ignore.
        return;
    }
    if (block_level >= 32) {
        throw std::runtime_error("Bad block level");
    }
    display[block_level] = value;
    machine.trace_display(block_level, value);
}

unsigned Processor::get_display(unsigned block_level) const
{
    if (block_level == 0) {
        // Global level: ignore.
        return 0;
    }
    if (block_level >= 32) {
        throw std::runtime_error("Bad block level");
    }
    return display[block_level];
}

//
// Update all display[] entries.
// Unwind stack starting from current frame.
//
void Processor::update_display(unsigned level)
{
    unsigned fp = frame_ptr;
    while (level > 0) {
        set_display(level, fp);
        level--;
        fp = stack.get(fp + Frame_Offset::DISPLAY).get_addr();
    }
}

void Processor::make_storage_function_frame(int elt_size)
{
    int nitems = core.S;
    int ndim   = (stack.count() - stack_base) / 2;
    std::vector<std::pair<int, int>> dims;
    for (int i = 0; i < ndim; ++i) {
        int right = stack.pop_integer();
        int left  = stack.pop_integer();
        dims.push_back(std::make_pair(left, right));
    }
    std::reverse(dims.begin(), dims.end());
    if (elt_size == 1)
        stack.push_int_addr(0); // future base address
    else
        stack.push_real_addr(0);
    stack.push_null(); // future "0 element" address
    unsigned zero_base = stack.count() - 1;
    int offset         = 0;
    int stride         = elt_size;
    for (int i = 0; i < ndim; ++i) {
        stack.push_int_value(stride);
        offset += stride * dims[i].first;
        stride *= (dims[i].second - dims[i].first + 1);
    }
    stack.set_int_value(zero_base, integer_to_x1(offset));
    stack.push_int_value(integer_to_x1(-stride));
    // Replicate the storage function (nitems-1) times.
    for (int i = 0; i < nitems - 1; ++i) {
        for (int j = 0; j < ndim + 3; ++j) {
            stack.push(stack.get(stack_base + j));
        }
    }
    stack_base += nitems * (ndim + 3);
}

Word Processor::load_word(unsigned addr)
{
    if (addr < STACK_BASE)
        return machine.mem_load(addr);
    else
        return stack.get(addr - STACK_BASE).get_int();
}

void Processor::make_value_array_function_frame(int elt_size)
{
    formal_mode = Formal_Op::PUSH_ADDRESS;
    push_formal_value(core.S);
    formal_mode = Formal_Op::PUSH_VALUE;
    unsigned storage_fn = stack.pop_addr();
    unsigned addr       = load_word(storage_fn);
    if (elt_size == 2) {
        stack.push_real_addr(addr);
        stack.push_real_addr(load_word(storage_fn + 1));
    } else {
        stack.push_int_addr(addr);
        stack.push_int_addr(load_word(storage_fn + 1));
    }
    bool seen_limit = false;
    for (int i = 2; i < 8; ++i) {
        auto w  = load_word(storage_fn + i);
        int val = x1_to_integer(w);
        if (i == 2 && val != elt_size) {
            // There are two possibilities for val and elt-size: (1, 2) and (2, 1),
            // to indicate, we use 21 for real to integer, 12 for integer to real, as the
            // (impossible) element size.
            w = val * 10 + elt_size;
        }
        stack.push_int_value(w);
        seen_limit |= val < 0 && -val <= 32767;
    }
    if (!seen_limit) {
        throw std::runtime_error("Too many dimensions for value array");
    }
    stack_base += 8;
}
