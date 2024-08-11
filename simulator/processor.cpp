#include "machine.h"
#include "opc.h"

#include <cmath>
#include <iostream>
#include <sstream>

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
    switch (opcode) {
    case 0:
        done_flag = call_opc(addr);
        break;

    case 002'20:
        core.A = addr;
        break;

    case 012'20:
        core.S = addr;
        break;

    case 042'20:
        core.B = addr;
        break;

    case 052'20:
        OT = addr;
        break;

    case 052'23:
        if (!core.C) {
            OT = addr;
        }
        break;

    //TODO: process other instructions

    default:
        // Unknown instruction - cannot happen.
        throw std::runtime_error("Unknown instruction " + to_octal(OR));
    }

    return done_flag;
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
    //TODO: case OPC_ETMR: // extransmark result
    case OPC_ETMP:
        // extransmark procedure
        // Invoke a procedure which address is located in register B.
        // Number of arguments is present in register A.
        // Save return address on stack.
        // Note: descriptors of procedure arguments are located
        // in memory 3 words before the return address.
        frame_create(OT, 0, core.A);
        OT = core.B;
        break;

    //TODO: case OPC_FTMR: // formtransmark result
    //TODO: case OPC_FTMP: // formtransmark procedure
    case OPC_RET:
        // return from procedure
        // Jump to address from stack.
        OT = frame_release();
        break;
    case OPC_EIS: {
        // end of implicit subroutine
        auto item = stack.pop();
        OT = frame_release();
        stack.push(item);
        break;
    }
    case OPC_TRAD: {
        // take real address dynamic
        // Dynamic address is present in register S.
        // Convert it to static address and push on stack.
        unsigned addr = static_address(core.S);
        stack.push_real_addr(addr);
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
        unsigned addr = static_address(core.S);
        stack.push_int_addr(addr);
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
        // Read word from memory at this address - it contains descriptor
        // of actual argument. It can hold either address of value, or address
        // of implicit subroutine. Call it to obtain actual argument value.
        unsigned arg = arg_descriptor(core.S);
        switch (arg >> 18 & 7) {
        case 0: {
            // Get real address.
            stack.push_real_addr(arg);
            break;
        }
        case 2: {
            // Get integer address.
            stack.push_int_addr(arg);
            break;
        }
        default:
            // Call implicit subroutine.
            frame_create(OT, 0, 0);
            OT = arg;
            break;
        }
        break;
    }

    //TODO: case OPC_FOR0:
    //TODO: case OPC_FOR1:
    //TODO: case OPC_FOR2:
    //TODO: case OPC_FOR3:
    //TODO: case OPC_FOR4:

    //TODO: case OPC_FOR5:
    //TODO: case OPC_FOR6:
    //TODO: case OPC_FOR7:
    //TODO: case OPC_FOR8:

    //TODO: case OPC_GTA: // goto adjustment
    //TODO: case OPC_SSI: // store switch index
    case OPC_CAC:
        // copy boolean acc. into condition
        core.C = stack.pop_boolean();
        break;

    case OPC_TRRD: {
        // take real result dynamic
        // Dynamic address is present in register S.
        // Convert it to static address, read real value and push on stack.
        unsigned addr = static_address(core.S);
        Word hi       = machine.mem_load(addr);
        Word lo       = machine.mem_load(addr + 1);
        Real value    = x1_words_to_real(hi, lo);
        stack.push_real_value(value);
        break;
    }
    case OPC_TRRS: {
        // Take real result static.
        // Read two words from memory at address in register B.
        // Push them in stack as real value.
        Word hi    = machine.mem_load(core.B);
        Word lo    = machine.mem_load(core.B + 1);
        Real value = x1_words_to_real(hi, lo);
        stack.push_real_value(value);
        break;
    }
    case OPC_TIRD: {
        // take integer result dynamic
        // Dynamic address is present in register S.
        // Convert it to static address, read integer value and push on stack.
        unsigned addr = static_address(core.S);
        Word value = machine.mem_load(addr);
        stack.push_int_value(value);
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
    case OPC_TFR: {
        // take formal result
        // Dynamic address is present in register S.
        // Read word from memory at this address - it contains descriptor
        // of actual argument. It can hold either address of value, or address
        // of implicit subroutine. Call it to obtain actual argument value.
        unsigned arg = arg_descriptor(core.S);
        switch (arg >> 18 & 7) {
        case 0: {
            // Get real value.
            Word hi    = machine.mem_load(arg);
            Word lo    = machine.mem_load(arg + 1);
            Real value = x1_words_to_real(hi, lo);
            stack.push_real_value(value);
            break;
        }
        case 2: {
            // Get integer value.
            Word value = machine.mem_load(arg);
            stack.push_int_value(value);
            break;
        }
        default:
            // Call implicit subroutine.
            frame_create(OT, 0, 0);
            OT = arg;
            break;
        }
        break;
    }
    //TODO: case OPC_ADRD: // add real dynamic
    case OPC_ADRS: {
        // add real static
        Word hi   = machine.mem_load(core.B);
        Word lo   = machine.mem_load(core.B + 1);
        auto b    = x1_to_ieee(x1_words_to_real(hi, lo));
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
    //TODO: case OPC_ADID: // add integer dynamic
    case OPC_ADIS: {
        // add integer static
        auto b = x1_to_integer(machine.mem_load(core.B));
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
    //TODO: case OPC_ADF:  // add formal

    //TODO: case OPC_SURD: // subtract real dynamic
    case OPC_SURS: {
        // subtract real static
        Word hi   = machine.mem_load(core.B);
        Word lo   = machine.mem_load(core.B + 1);
        auto b    = x1_to_ieee(x1_words_to_real(hi, lo));
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
    //TODO: case OPC_SUID: // subtract integer dynamic
    case OPC_SUIS: {
        // subtract integer static
        auto b = x1_to_integer(machine.mem_load(core.B));
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
    //TODO: case OPC_SUF:  // subtract formal

    //TODO: case OPC_MURD: // multiply real dynamic
    case OPC_MURS: {
        // multiply real static
        Word hi   = machine.mem_load(core.B);
        Word lo   = machine.mem_load(core.B + 1);
        auto b    = x1_to_ieee(x1_words_to_real(hi, lo));
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
    //TODO: case OPC_MUID: // multiply integer dynamic
    case OPC_MUIS: {
        // multiply integer static
        auto b = x1_to_integer(machine.mem_load(core.B));
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
    //TODO: case OPC_MUF:  // static multiply formal

    //TODO: case OPC_DIRD: // divide real dynamic
    case OPC_DIRS: {
        // divide real static
        Word hi = machine.mem_load(core.B);
        Word lo = machine.mem_load(core.B + 1);
        auto b  = x1_to_ieee(x1_words_to_real(hi, lo));
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = (long double) x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a / b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a / b));
        } else {
            throw std::runtime_error("Cannot multiply address by real");
        }
        break;
    }
    //TODO: case OPC_DIID: // divide integer dynamic
    case OPC_DIIS: {
        // divide integer static
        auto b = x1_to_integer(machine.mem_load(core.B));
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        auto item = stack.pop();
        if (item.is_int_value()) {
            auto a = (long double) x1_to_integer(item.get_int());
            stack.push_real_value(ieee_to_x1(a / b));
        } else if (item.is_real_value()) {
            auto a = x1_to_ieee(item.get_real());
            stack.push_real_value(ieee_to_x1(a / b));
        } else {
            throw std::runtime_error("Cannot divide address by integer");
        }
        break;
    }
    //TODO: case OPC_DIF:  // divide formal

    //TODO: case OPC_IND: // indexer

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
    //TODO: case OPC_TAR: // take result
    //TODO: case OPC_ADD: // add
    //TODO: case OPC_SUB: // subtract
    //TODO: case OPC_MUL: // multiply
    //TODO: case OPC_DIV: // divide
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
        Word result;
        if (item.is_int_value()) {
            auto value = x1_to_integer(item.get_int());
            result = (value > 0) ? 1 : (value < 0) ? x1_negate_int(1) : 0;
        } else if (item.is_real_value()) {
            auto value = x1_to_ieee(item.get_real());
            result = (value > 0.0) ? 1 : (value < 0.0) ? x1_negate_int(1) : 0;
        } else {
            throw std::runtime_error("Cannot get sign of address");
        }
        stack.push_int_value(result);
        break;
    }
    case OPC_sqrt: {
        // Function sqrt(E) - square root of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        long double input = stack.pop_ieee();
        if (input < 0) {
            throw std::runtime_error("Cannot get square root of negative value " + std::to_string(input));
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
            throw std::runtime_error("Cannot get logarithm of non-positive value " + std::to_string(input));
        }
        stack.push_ieee(logl(input));
        break;
    }
    case OPC_exp: {
        // Function exp(E) - exponential function of the value of E (e**E)
        // Yield value of type real.
        // Argument can be either of type real or integer.
        long double input = stack.pop_ieee();
        long double result = expl(input);
        if (result == HUGE_VALL) {
            throw std::runtime_error("Overflow in exponential function of value " + std::to_string(input));
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
            result = integer_to_x1((int) floorl(input));
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
    //TODO: case OPC_STA:  // store also
    //TODO: case OPC_STP:  // store procedure value
    //TODO: case OPC_STAP: // store also procedure value

    case OPC_SCC:
        // short circuit
        // Numeric argument is present in register B.
        if (core.B > 31) {
            throw std::runtime_error("Bad block level in SCC");
        }
        display[core.B] = frame_ptr;
        machine.trace_display(core.B, frame_ptr);
        stack_base = stack.count();
        break;
    //TODO: case OPC_RSF: // real arrays storage function frame
    //TODO: case OPC_ISF: // integer arrays storage function frame
    //TODO: case OPC_RVA: // real value array storage function frame
    //TODO: case OPC_IVA: // integer value array storage function frame
    //TODO: case OPC_LAP: // local array positioning
    //TODO: case OPC_VAP: // value array positioning

    case OPC_START:
        // Start of the object program.
        break;

    case OPC_STOP:
        // End of the object program.
        return true;

    //TODO: case OPC_TFP:  // take formal parameter
    //TODO: case OPC_TAS:  // type algol symbol
    //TODO: case OPC_OBC6: // output buffer class 6
    //TODO: case OPC_FLOATER:
    //TODO: case OPC_read:

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
    //TODO: case OPC_TAB:
    //TODO: case OPC_NLCR:
    //TODO: case OPC_XEEN:
    //TODO: case OPC_SPACE:
    //TODO: case OPC_stop:
    //TODO: case OPC_P21:

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
void Processor::frame_create(unsigned ret_addr, unsigned result_addr, unsigned num_args)
{
    auto new_frame_ptr = stack.count();

    stack.push_int_addr(frame_ptr);   // offset 0: previos frame pointer
    stack.push_int_addr(ret_addr);    // offset 1: return address
    stack.push_int_addr(result_addr); // offset 2: address to store result
    stack.push_int_addr(stack_base);  // offset 3: base of the stack

    for (unsigned i = 0; i < num_args; i++) {
        // Allocate formal parameters: offset 4, 5 and so on/
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
    auto ret_addr = stack.get(frame_ptr + 1).get_addr();
    stack_base    = stack.get(frame_ptr + 3).get_addr();
    frame_ptr     = stack.get(frame_ptr).get_addr();
    stack.erase(new_stack_ptr);
    return ret_addr;
}

//
// Convert dynamic address of variable (relative to stack frame)
// into static address.
//
// Split dynamic address into block level and memory offset.
// For example, S=0241=161 means 5*32+1.
// Here 1 is the block level, and 5 is offset.
// Get frame pointer of required block from display[block_level].
// Add offset (plus some correction).
// This is static address.
//
unsigned Processor::static_address(unsigned dynamic_addr)
{
    auto const offset      = dynamic_addr / 32;
    auto const block_level = dynamic_addr % 32;
    auto const correction  = -1;
    auto const static_addr = display[block_level] + offset + correction;

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
    auto const offset      = dynamic_addr / 32;
    auto const block_level = dynamic_addr % 32;
    auto const ret_addr    = stack.get(display[block_level] + 1).get_addr();
    auto const arg_descr   = machine.mem_load(ret_addr + offset - 8);

    return arg_descr;
}

//
// Store value given by src cell.
// Write it to memory address given by dest cell.
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
            result = integer_to_x1((int) roundl(x1_to_ieee(src.get_real())));
        } else {
            throw std::runtime_error("Cannot store address");
        }
        machine.mem_store(addr, result);
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
                result = ieee_to_x1((long double) x1_to_integer(value));
            }
        } else {
            throw std::runtime_error("Cannot store address");
        }
        machine.mem_store(addr, (result >> 27) & BITS(27));
        machine.mem_store(addr + 1, result & BITS(27));
        break;
    }
    default:
        throw std::runtime_error("Bad destination");
    }

}
