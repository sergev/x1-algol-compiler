#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

#include "encoding.h"
#include "machine.h"
#include "opc.h"

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
        core.B = machine.mem_load((addr + core.B) & BITS(15));
        break;

    case 046'00:
        machine.mem_store(addr, core.B);
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
    Word hi = machine.mem_load(addr);
    Word lo = machine.mem_load(addr + 1);
    return x1_words_to_real(hi, lo);
}

static inline int x1_sign(const Stack_Cell & item) {
    if (item.is_int_value()) {
        auto value = x1_to_integer(item.get_int());
        return (value > 0) ? 1 : (value < 0) ? -1 : 0;
    } else if (item.is_real_value()) {
        auto value = x1_to_ieee(item.get_real());
        return (value > 0.0) ? 1 : (value < 0.0) ? -1 : 0;
    } else {
        throw std::runtime_error("Cannot get sign of address");
    }
}

Stack_Cell Processor::load_value(const Stack_Cell & src) {
    auto addr = src.get_addr();
    if (src.is_int_addr()) {
        Word result;
        if (addr < STACK_BASE) {
            // From memory.
            result = machine.mem_load(addr);
        } else {
            // From stack.
            result = stack.get(addr - STACK_BASE).get_int();
        }
        return Stack_Cell{Cell_Type::INTEGER_VALUE, result};
    } else if (src.is_real_addr()) {
        Real result;
        if (addr < STACK_BASE) {
            // From memory.
            result = load_real(addr);
        } else {
            // From stack.
            result = stack.get(addr - STACK_BASE).get_real();
        }
        return Stack_Cell{Cell_Type::REAL_VALUE, result};
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
// Invoke a run-time routine by code.
//
// "The translator formulates the object program in a number of
// standard operations, which are numbered by a so-called Output
// Code (OPC)." (E.W.Dijkstra)
//
bool Processor::call_opc(unsigned opc)
{
    switch (opc) {
    case OPC_ETMR:
        // extransmark result
        // Invoke a function which address is located in register B.
        // Number of arguments is present in register A.
        frame_create(OT, core.A);
        OT = core.B;

        // Expect result on return from procedure.
        stack.get(frame_ptr + Frame_Offset::RESULT).type = Cell_Type::INTEGER_VALUE;
        break;

    case OPC_ETMP:
        // extransmark procedure
        // Invoke a procedure which address is located in register B.
        // Number of arguments is present in register A.
        // Save return address on stack.
        // Note: descriptors of procedure arguments are located
        // in memory 3 words before the return address.
        machine.mem_store(51, OT - 8); // for PRINTTEXT
        frame_create(OT, core.A);
        OT = core.B;
        break;

    // TODO: case OPC_FTMR: // formtransmark result
    // TODO: case OPC_FTMP: // formtransmark procedure

    case OPC_FOR8:
        // Drop the unneeded address of the controlled variable.
        stack.pop();
        // The loop subroutine returns nothing.
        stack.set(frame_ptr + Frame_Offset::RESULT, Stack_Cell{Cell_Type::NUL, 0});

        // Return from the loop subroutine using the provided destination.
        stack.set(frame_ptr + Frame_Offset::PC,
                  Stack_Cell{Cell_Type::INTEGER_ADDRESS, core.S});
        // In the complex, FOR8 jumps to RET.
        // FALLTHRU

    case OPC_RET: {
        // return from procedure
        // Jump to address from stack.
        // Restore display[n].
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null()) {
            unsigned block_level = disp.value >> 15;
            display[block_level] = disp.value & BITS(15);
            machine.trace_display(block_level, display[block_level]);
        }
        auto result = stack.get(frame_ptr + Frame_Offset::RESULT);
        OT          = frame_release();
        if (!result.is_null()) {
            // Push result on stack.
            stack.push(result);
        }
        break;
    }
    case OPC_EIS: {
        // end of implicit subroutine
        auto item = stack.pop();
        OT        = frame_release();
        stack.push(item);

        // Restore display[n].
        if (stack_base > 0) {
            auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
            if (!disp.is_null()) {
                unsigned block_level = disp.value >> 15;
                display[block_level] = frame_ptr;
                machine.trace_display(block_level, frame_ptr);
            }
        }
        break;
    }
    case OPC_TRAD: {
        // take real address dynamic
        // Dynamic address is present in register S.
        // Convert it to static address and push on stack.
        unsigned addr = address_in_stack(core.S);
        stack.push_real_addr(addr + STACK_BASE);
        break;
    }
    case OPC_TRAS: {
        // take real address static
        // The static address is in register B.
        stack.push_real_addr(core.B);
        break;
    }
    case OPC_TIAD: {
        // take integer address dynamic
        // Dynamic address is present in register S.
        // Convert it to static address and push on stack.
        unsigned addr = address_in_stack(core.S);
        stack.push_int_addr(addr + STACK_BASE);
        break;
    }
    case OPC_TIAS: {
        // take integer address static
        // The static address is in register B.
        stack.push_int_addr(core.B);
        break;
    }
    case OPC_TFA: {
        // take formal address
        // Dynamic address is present in register S.
        push_formal_address(core.S);
        break;
    }

    case OPC_FOR1:
        // Save the execution link (which points to a jump to the loop body)
        // in the local variable.
        stack.set(frame_ptr + Frame_Offset::ARG,
                  Stack_Cell{Cell_Type::INTEGER_ADDRESS, OT});
        // When starting to execute a new loop element,
        // jump to it (initially PC points to under the ETMP for the loop,
        // then it gets advanced by the OPCs of the loop elements
        // as they get exhausted).
        OT = stack.get(frame_ptr + Frame_Offset::PC).get_addr();
        break;

    case OPC_FOR2: {
        // The element does not repeat.
        stack.set(frame_ptr + Frame_Offset::PC,
                  Stack_Cell{Cell_Type::INTEGER_ADDRESS, OT});
        // Store the next value of the loop variable.
        auto src  = stack.pop();
        auto dest = stack.pop();
        store_value(dest, src);
        OT = stack.get(frame_ptr + Frame_Offset::ARG).get_addr();        
        break;
    }

    case OPC_FOR3: {
        // Store the next value of the loop variable.
        auto src  = stack.pop();
        auto dest = stack.pop();
        store_value(dest, src);
        stack.push(dest);        // for the next element in case of exhaustion
        break;
    }

    case OPC_FOR4:
        // Check the while condition.
        core.C = stack.pop_boolean();
        if (core.C) {
            stack.pop();        // address pushed by FOR3 not needed
            // Go to the loop iteration.
            OT = stack.get(frame_ptr + Frame_Offset::ARG).get_addr();
        } else {
            // Indicate end of the 'while' element, continue to the next one.
            stack.set(frame_ptr + Frame_Offset::PC,
                      Stack_Cell{Cell_Type::INTEGER_ADDRESS, OT});
        }
        break;

    case OPC_FOR5:
        // Indicate entering the step-until element.
        stack.set(frame_ptr + Frame_Offset::RESULT,
                  Stack_Cell{Cell_Type::NUL, 0});
        // The job of FOR5 is done.
        stack.set(frame_ptr + Frame_Offset::PC,
                  Stack_Cell{Cell_Type::INTEGER_ADDRESS, OT});
        break;

    case OPC_FOR6: {
        auto incr = stack.pop();
        int step_dir = x1_sign(incr);
        if (stack.get(frame_ptr + Frame_Offset::RESULT).is_null()) {
            // First iteration: the actual value of the step is ignored,
            // we're only interested in its sign.
            // The initial value is now on top of the stack, pending FOR7 check.
        } else {
            // A subsequent iteration: load and increment the controlled variable.
            auto var = stack.pop();
            auto value = load_value(var);
            value.add(incr);
            // Prepare the stack for storing the new value of the variable.
            stack.push(var);
            stack.push(value);
        }
        stack.set(frame_ptr + Frame_Offset::RESULT,
                  Stack_Cell{Cell_Type::INTEGER_VALUE, integer_to_x1(step_dir)});
        break;
    }

    case OPC_FOR7: {
        auto limit = stack.pop();
        auto value = stack.pop();
        auto var = stack.pop();
        store_value(var, value);
        int step_dir = x1_to_integer(stack.get(frame_ptr + Frame_Offset::RESULT).get_int());
        if ((step_dir > 0 && limit.is_less(value)) ||
            (step_dir < 0 && value.is_less(limit))) {
            // Element exhausted.
            stack.push(var);    // For the sake of the next loop element.
            stack.set(frame_ptr + Frame_Offset::PC,
                      Stack_Cell{Cell_Type::INTEGER_ADDRESS, OT});
        } else {
            // Go to the loop iteration.
            OT = stack.get(frame_ptr + Frame_Offset::ARG).get_addr();
        }
        break;
    }

    // TODO: case OPC_GTA: // goto adjustment
    // TODO: case OPC_SSI: // store switch index
    case OPC_CAC:
        // copy boolean acc. into condition
        core.C = stack.pop_boolean();
        break;

    case OPC_TRRD: {
        // take real result dynamic
        // Dynamic address is present in register S.
        // Convert it to stack offset, read real value and push on stack.
        auto addr   = address_in_stack(core.S);
        auto result = stack.get(addr);
        if (!result.is_real_value()) {
            throw std::runtime_error("Wrong result type in TRRD");
        }
        stack.push(result);
        break;
    }
    case OPC_TRRS: {
        // Take real result static.
        // Read two words from memory at address in register B.
        // Push them in stack as real value.
        stack.push_real_value(load_real(core.B));
        break;
    }
    case OPC_TIRD: {
        // take integer result dynamic
        // Dynamic address is present in register S.
        // Convert it to stack offset, read integer value and push on stack.
        unsigned addr = address_in_stack(core.S);
        auto result   = stack.get(addr);
        if (!result.is_int_value()) {
            throw std::runtime_error("Wrong result type in TIRD");
        }
        stack.push(result);
        break;
    }
    case OPC_TIRS: {
        // Take integer result static.
        // Read word from memory at address in register B.
        // Push it in stack as integer value.
        Word value = machine.mem_load(core.B);
        stack.push_int_value(value);
        break;
    }
    case OPC_TFR:
        // take formal result
        // Dynamic address is present in register S.
        push_formal_value(core.S);
        break;
    case OPC_ADRD: {
        // add real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().add(b);
        break;
    }
    case OPC_ADRS: {
        // add real static
        auto b    = x1_to_ieee(load_real(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a + b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a + b));
        } else {
            throw std::runtime_error("Cannot add real to address");
        }
        break;
    }
    case OPC_ADID: {
        // add integer dynamic
        auto b = get_dynamic_int(core.S);
        stack.top().add(b);
        break;
    }
    case OPC_ADIS: {
        // add integer static
        auto b    = x1_to_integer(machine.mem_load(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_int_value(integer_to_x1(a + b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a + b));
        } else {
            throw std::runtime_error("Cannot add integer to address");
        }
        break;
    }
    case OPC_ADF: {
        // add formal
        // Left argument is on stack.
        // Register S has fynamic address of right argument.
        push_formal_value(core.S);
        auto b = stack.pop();
        auto a = stack.pop();
        a.add(b);
        stack.push(a);
        break;
    }
    case OPC_SURD: {
        // subtract real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().subtract(b);
        break;
    }
    case OPC_SURS: {
        // subtract real static
        auto b    = x1_to_ieee(load_real(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a - b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a - b));
        } else {
            throw std::runtime_error("Cannot subtract real from address");
        }
        break;
    }
    case OPC_SUID: {
        // subtract integer dynamic
        auto b = get_dynamic_int(core.S);
        stack.top().subtract(b);
        break;
    }
    case OPC_SUIS: {
        // subtract integer static
        auto b    = x1_to_integer(machine.mem_load(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_int_value(integer_to_x1(a - b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a - b));
        } else {
            throw std::runtime_error("Cannot subtract integer from address");
        }
        break;
    }
        // TODO: case OPC_SUF:  // subtract formal

    case OPC_MURD: {
        // multiply real dynamic
        // Dynamic address of second argument is present in register S.
        auto addr = address_in_stack(core.S);
        auto b    = stack.get(addr);
        if (!b.is_real_value()) {
            throw std::runtime_error("Wrong second argument in MURD");
        }
        auto a = stack.pop();
        a.multiply(b);
        stack.push(a);
        break;
    }
    case OPC_MURS: {
        // multiply real static
        auto b    = x1_to_ieee(load_real(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a * b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a * b));
        } else {
            throw std::runtime_error("Cannot multiply address by real");
        }
        break;
    }
    case OPC_MUID: {
        // multiply integer dynamic
        auto b = get_dynamic_int(core.S);
        stack.top().multiply(b);
        break;
    }
    case OPC_MUIS: {
        // multiply integer static
        auto b    = x1_to_integer(machine.mem_load(core.B));
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = x1_to_integer(item.get_int());
            stack.push_int_value(integer_to_x1(a * b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a * b));
        } else {
            throw std::runtime_error("Cannot multiply address by integer");
        }
        break;
    }
    // TODO: case OPC_MUF:  // static multiply formal

    case OPC_DIRD: {
        // divide real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().divide(b);
        break;
    }
    case OPC_DIRS: {
        // divide real static
        auto b = x1_to_ieee(load_real(core.B));
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = (long double)x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a / b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a / b));
        } else {
            throw std::runtime_error("Cannot multiply address by real");
        }
        break;
    }
    case OPC_DIID: {
        // divide integer dynamic
        auto b = get_dynamic_int(core.S);
        stack.top().divide(b);
        break;
    }
    case OPC_DIIS: {
        // divide integer static
        auto b = x1_to_integer(machine.mem_load(core.B));
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = (long double)x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a / b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a / b));
        } else {
            throw std::runtime_error("Cannot divide address by integer");
        }
        break;
    }
        // TODO: case OPC_DIF:  // divide formal

    case OPC_IND: {
        // The top of the stack is: storage function address, idx1 [idx2 [idx3 [...] ] ]
        std::vector<Stack_Cell> idxs;
        Stack_Cell storage_fn;
        do {
            auto item = stack.pop();
            if (item.is_int_addr() || item.is_real_addr()) {
                storage_fn = item;
                break;
            }
            idxs.push_back(item);
        } while (true);
        if (idxs.empty()) {
            throw std::runtime_error("Indexing but no indexes given");
        }
        std::reverse(idxs.begin(), idxs.end());
        unsigned ndim = idxs.size();
        // The storage function of an array is described by 3+ndim words.
        // For a 1-dimensional array, "offset: contents":
        // 0: address of the first memory word the array occupies
        // 1: base address for indexing from 0 (for a left bound >= 0)
        //    (with the high bit set for a left bound < 0 - a 1's complement artifact)
        // 2..2+ndim-1: element or dimension sizes in words
        // 2+ndim: number of words in the array, negated
        unsigned addr = storage_fn.get_addr();
        int location  = x1_to_integer(machine.mem_load(addr));
        int base      = machine.mem_load(addr + 1);
        int limit     = x1_to_integer(machine.mem_load(addr + 2 + ndim));
        // The limit must look like a negative number with a reasonable absolute value.
        if (limit >= 0 || -limit > 32767) {
            throw std::runtime_error("A wrong number of indexes for an array");
        }
        base = (base & BITS(26)) + (base >> 26);
        int elt_addr = base;
        int idx0     = 0; // for the error message
        for (unsigned i = 0; i < ndim; ++i) {
            int dimsize = x1_to_integer(machine.mem_load(addr + 2 + i));
            int idx = idxs[i].is_int_value() ?
                x1_to_integer(idxs[i].get_int()) :
                (int)roundl(x1_to_ieee(idxs[i].get_real()));
            if (i == 0)
                idx0 = idx;
            elt_addr += idx * dimsize;
        }
        elt_addr &= BITS(16); // 16 rather than 15 is to catch "negative" addresses
        if (elt_addr < location || elt_addr + limit >= location) {
            std::ostringstream ostr;
            int elsize = x1_to_integer(machine.mem_load(addr + 2));
            if (ndim == 1) {
                ostr << "Index " << idx0 << " is ";
            } else {
                ostr << "Indexing (linearized " << elt_addr - base << ") ";
            }
            ostr << "beyond limits for array [" << (location - base) / elsize << ':'
                 << (location - base - limit) / elsize - 1 << ']';
            throw std::runtime_error(ostr.str());
        }
        if (storage_fn.is_int_addr())
            stack.push_int_addr(elt_addr);
        else
            stack.push_real_addr(elt_addr);
        break;
    }

    case OPC_NEG: {
        // Invert sign accumulator.
        auto item = stack.pop();
        if (item.is_int_value()) {
            stack.push_int_value(x1_negate_int(item.get_int()));
        } else if (item.is_real_value()) {
            stack.push_real_value(x1_negate_real(item.get_real()));
        } else {
            throw std::runtime_error("Cannot negate address");
        }
        break;
    }
    case OPC_TAR: {
        // take result
        auto src  = stack.pop();
        stack.push(load_value(src));
        break;
    }
    case OPC_ADD: {
        // add
        auto b = stack.pop();
        auto a = stack.pop();
        a.add(b);
        stack.push(a);
        break;
    }
    case OPC_SUB: {
        // subtract
        auto b = stack.pop();
        auto a = stack.pop();
        a.subtract(b);
        stack.push(a);
        break;
    }
    case OPC_MUL: {
        // multiply
        auto b = stack.pop();
        auto a = stack.pop();
        a.multiply(b);
        stack.push(a);
        break;
    }
    case OPC_DIV: {
        // divide
        auto b = stack.pop();
        auto a = stack.pop();
        a.divide(b);
        stack.push(a);
        break;
    }
    case OPC_IDI: {
        // integer division
        auto b = stack.pop_integer();
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        auto a = stack.pop_integer();
        stack.push_int_value(integer_to_x1(a / b));
        break;
    }
    case OPC_TTP: {
        // to the power
        auto b = stack.pop();
        stack.top().exponentiate(b);
        break;
    }

    case OPC_MOR: {
        // more >
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(b.is_less(a) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_LST: {
        // at least ≥
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(!a.is_less(b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_EQU: {
        // equal =
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(a.is_equal(b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_MST: {
        // at most ≤
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(!b.is_less(a) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_LES: {
        // less <
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(a.is_less(b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_UQU: {
        // unequal ≠
        auto b = stack.pop();
        auto a = stack.pop();
        stack.push_int_value(!a.is_equal(b) ? X1_TRUE : X1_FALSE);
        break;
    }

    case OPC_NON: {
        // non ¬
        bool input = stack.pop_boolean();
        stack.push_int_value(input ? X1_FALSE : X1_TRUE);
        break;
    }
    case OPC_AND: {
        // and ∧
        bool b = stack.pop_boolean();
        bool a = stack.pop_boolean();
        stack.push_int_value((a & b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_OR: {
        // or ∨
        bool b = stack.pop_boolean();
        bool a = stack.pop_boolean();
        stack.push_int_value((a | b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_IMP: {
        // implies ⊃
        bool b = stack.pop_boolean();
        bool a = stack.pop_boolean();
        stack.push_int_value((!a || b) ? X1_TRUE : X1_FALSE);
        break;
    }
    case OPC_QVL: {
        // equivalent ≡
        bool b = stack.pop_boolean();
        bool a = stack.pop_boolean();
        stack.push_int_value((a == b) ? X1_TRUE : X1_FALSE);
        break;
    }

    case OPC_abs: {
        // Function abs(E) - the modulus (absolute value) of the value of the expression E.
        // Yield value of type real.
        // Argument can be either of type real or integer.
        auto item = stack.pop();
        Real result;
        if (item.is_int_value()) {
            result = ieee_to_x1(x1_abs_int(item.get_int()));
        } else if (item.is_real_value()) {
            result = x1_abs_real(item.get_real());
        } else {
            throw std::runtime_error("Cannot get absolute value of address");
        }
        stack.push_real_value(result);
        break;
    }
    case OPC_sign: {
        // Function sign(E) - the sign of the value of E (+1 for E > 0, 0 for E = 0, −1 for E < 0)
        // Yield value of type integer.
        // Argument can be either of type real or integer.
        auto item = stack.pop();
        Word result = integer_to_x1(x1_sign(item));
        stack.push_int_value(result);
        break;
    }
    case OPC_sqrt: {
        // Function sqrt(E) - square root of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        long double input = stack.pop_ieee();
        if (input < 0) {
            throw std::runtime_error("Cannot get square root of negative value " +
                                     std::to_string(input));
        }
        stack.push_ieee(sqrtl(input));
        break;
    }
    case OPC_sin: {
        // Function sin(E) - sine of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        stack.push_ieee(sinl(stack.pop_ieee()));
        break;
    }
    case OPC_cos: {
        // Function cos(E) - cosine of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        stack.push_ieee(cosl(stack.pop_ieee()));
        break;
    }
    case OPC_ln: {
        // Function ln(E) - natural logarithm of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        long double input = stack.pop_ieee();
        if (input <= 0) {
            throw std::runtime_error("Cannot get logarithm of non-positive value " +
                                     std::to_string(input));
        }
        stack.push_ieee(logl(input));
        break;
    }
    case OPC_exp: {
        // Function exp(E) - exponential function of the value of E (e**E)
        // Yield value of type real.
        // Argument can be either of type real or integer.
        long double input  = stack.pop_ieee();
        long double result = expl(input);
        if (result == HUGE_VALL) {
            throw std::runtime_error("Overflow in exponential function of value " +
                                     std::to_string(input));
        }
        stack.push_ieee(result);
        break;
    }
    case OPC_entier: {
        // Function entier(E) - largest integer not greater than the value of E.
        // Yield value of type integer.
        // Argument can be either of type real or integer.
        auto item = stack.pop();
        Word result;
        if (item.is_int_value()) {
            result = item.get_int();
        } else if (item.is_real_value()) {
            long double input = x1_to_ieee(item.get_real());
            if (input < -(int)BITS(26) || input >= (int)BITS(26) + 1) {
                throw std::runtime_error("Overflow in entier of value " + std::to_string(input));
            }
            result = integer_to_x1((int)floorl(input));
        } else {
            throw std::runtime_error("Cannot get entier of address");
        }
        stack.push_int_value(result);
        break;
    }

    case OPC_ST: {
        // store
        auto src  = stack.pop();
        auto dest = stack.pop();
        store_value(dest, src);
        break;
    }

    case OPC_STA: {
        // store also
        auto src  = stack.pop();
        auto dest = stack.pop();
        store_value(dest, src);
        stack.push(src);
        break;
    }

    case OPC_STP: {
        // store procedure value
        // Block level is present in register B.
        // Zero means the current procedure.
        // Result is stored in stack frame at offset 2.
        // Pop result from stack.
        auto result = stack.pop();
        auto addr   = display[core.B + 1] + Frame_Offset::RESULT;
        if (!stack.get(addr).is_null()) {
            stack.set(addr, result);
        }
        break;
    }
    case OPC_STAP: {
        // store also procedure value
        // Block level is present in register B.
        // Zero means the current procedure.
        // Result is stored in stack frame at offset 2.
        // Leave a copy of result on stack.
        auto const &result = stack.top();
        auto addr          = display[core.B + 1] + Frame_Offset::RESULT;
        if (!stack.get(addr).is_null()) {
            stack.set(addr, result);
        }
        break;
    }

    case OPC_FOR0: {
        // Enters an implicit subroutine with one local word.
        // In the complex, B := BN + 1; where BN is M[52]
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        unsigned block_level = disp.is_null() ? 0 : disp.value >> 15;
        core.B = block_level + 1;
        allocate_stack(1);
        stack_base += 1;
        // In the complex, FOR0 jumps to SCC.
    }
        // FALLTHRU
    case OPC_SCC: {
        // short circuit
        // Numeric argument is present in register B.
        if (core.B > 31) {
            throw std::runtime_error("Bad block level in SCC");
        }
        // Save display[n] and block level.
        auto &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        disp.type  = Cell_Type::INTEGER_VALUE;
        disp.value = display[core.B] | (core.B << 15);

        display[core.B] = frame_ptr;
        machine.trace_display(core.B, frame_ptr);
        stack_base = stack.count();
        break;
    }
        // TODO: case OPC_RSF: // real arrays storage function frame
        // TODO: case OPC_ISF: // integer arrays storage function frame
        // TODO: case OPC_RVA: // real value array storage function frame
        // TODO: case OPC_IVA: // integer value array storage function frame
        // TODO: case OPC_LAP: // local array positioning
        // TODO: case OPC_VAP: // value array positioning

    case OPC_START:
        // Start of the object program.
        break;

    case OPC_STOP:
        // End of the object program.
        return true;

        // TODO: case OPC_TFP:  // take formal parameter

    case OPC_TAS:
        // type Algol symbol
        algol_putc(core.S, std::cout);
        break;
        // TODO: case OPC_OBC6: // output buffer class 6
        // TODO: case OPC_FLOATER:
        // TODO: case OPC_read:

    case OPC_print: {
        // Print number(s).
        if (stack.count() == stack_base) {
            // Emit empty line.
            std::cout << '\n';
        } else {
            for (auto i = stack_base; i < stack.count(); i++) {
                auto item = stack.get(i);
                if (item.is_int_value()) {
                    x1_print_integer(std::cout, item.get_int());
                } else if (item.is_real_value()) {
                    x1_print_real(std::cout, item.get_real());
                } else {
                    throw std::runtime_error("Cannot print address");
                }
                std::cout << '\n';
            }
            stack.erase(stack_base);
        }
        break;
    }

    case OPC_TAB:
        std::cout << '\t';
        break;

    case OPC_NLCR:
        std::cout << std::endl;
        break;

        // TODO: case OPC_XEEN:
        // TODO: case OPC_SPACE:
        // TODO: case OPC_stop:
        // TODO: case OPC_P21:

    default: {
        std::ostringstream ostr;
        x1_print_instruction(ostr, opc);
        throw std::runtime_error("NYI OPC " + ostr.str());
    }
    }
    return false;
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
    stack.push_null();               // offset 3: place for result
    stack.push_null();               // offset 4: place for display[n]

    for (unsigned i = 0; i < num_args; i++) {
        // Allocate formal parameters: two cells per parameter.
        stack.push_int_value(0);
        stack.push_int_value(0);
    }
    frame_ptr = new_frame_ptr;
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
    auto const offset      = dynamic_addr / 32;
    auto const block_level = dynamic_addr % 32;
    auto const static_addr = display[block_level] + offset;

    return static_addr;
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
    auto const ret_addr    = stack.get(display[block_level] + Frame_Offset::PC).get_addr();
    auto const arg_descr   = machine.mem_load(ret_addr - arg_num - 3);

    return arg_descr;
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
void Processor::push_formal_address(unsigned dynamic_addr)
{
    unsigned arg = arg_descriptor(core.S);
    switch (arg >> 15 & 077) {
    case 000: {
        // Get real address.
        stack.push_real_addr(arg);
        break;
    }
    case 020: {
        // Get integer address.
        stack.push_int_addr(arg);
        break;
    }
    case 040: {
        // Call implicit subroutine.
        // Restore display[n].
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null()) {
            unsigned block_level = disp.value >> 15;
            display[block_level] = disp.value & BITS(15);
            machine.trace_display(block_level, display[block_level]);
        }
        frame_create(OT, 0);
        machine.run(arg, OT);
        break;
    }
    case 002: {
        // Get real value from stack.
        auto block_level = arg >> 22;
        auto offset      = arg & BITS(15);
        auto addr        = display[block_level] + offset;
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null() && block_level == disp.value >> 15) {
            // Use saved display value.
            addr = (disp.value & BITS(15)) + offset;
        }
        stack.push_real_addr(addr + STACK_BASE);
        break;
    }
    case 022: {
        // Get integer value from stack.
        auto block_level = arg >> 22;
        auto offset      = arg & BITS(15);
        auto addr        = display[block_level] + offset;
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null() && block_level == disp.value >> 15) {
            // Use saved display value.
            addr = (disp.value & BITS(15)) + offset;
        }
        stack.push_int_addr(addr + STACK_BASE);
        break;
    }
    default:
        throw std::runtime_error("Unknown descriptor of formal argument: " + to_octal(arg));
    }
}

//
// Read word from memory at dynamic address - it contains descriptor
// of actual argument. It can hold either address of value, or address
// of implicit subroutine. Call it to obtain actual argument value.
//
void Processor::push_formal_value(unsigned dynamic_addr)
{
    unsigned arg = arg_descriptor(dynamic_addr);
    switch (arg >> 15 & 077) {
    case 000: {
        // Get real value from memory.
        stack.push_real_value(load_real(arg));
        break;
    }
    case 020: {
        // Get integer value from memory.
        Word value = machine.mem_load(arg);
        stack.push_int_value(value);
        break;
    }
    case 040: {
        // Call implicit subroutine.
        // Restore display[n].
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null()) {
            unsigned block_level = disp.value >> 15;
            display[block_level] = disp.value & BITS(15);
            machine.trace_display(block_level, display[block_level]);
        }
        frame_create(OT, 0);
        machine.run(arg, OT);
        break;
    }
    case 002:
        // Get real value from stack.
    case 022: {
        // Get integer value from stack.
        auto block_level = arg >> 22;
        auto offset      = arg & BITS(15);
        auto addr        = display[block_level] + offset;
        auto const &disp = stack.get(frame_ptr + Frame_Offset::DISPLAY);
        if (!disp.is_null() && block_level == disp.value >> 15) {
            // Use saved display value.
            addr = (disp.value & BITS(15)) + offset;
        }
        stack.push(stack.get(addr));
        break;
    }
    default:
        throw std::runtime_error("Unknown descriptor of formal argument: " + to_octal(arg));
    }
}
