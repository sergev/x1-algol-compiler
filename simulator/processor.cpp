#include "machine.h"
#include "opc.h"

#include <iostream>

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

    case 042'20:
        core.B = addr;
        break;

    case 052'20:
        OT = addr;
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
    //TODO: case OPC_ETMR: //  extransmark result
    //TODO: case OPC_ETMP: //  extransmark procedure
    //TODO: case OPC_FTMR: // formtransmark result
    //TODO: case OPC_FTMP: // formtransmark procedure
    //TODO: case OPC_RET: // return
    //TODO: case OPC_EIS: // end of implicit subroutine

    //TODO: case OPC_TRAD: // take real address dynamic
    //TODO: case OPC_TRAS: // take real address static
    //TODO: case OPC_TIAD: // take integer address dynamic
    //TODO: case OPC_TIAS: // take integer address static
    //TODO: case OPC_TFA: // take formal address

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
    //TODO: case OPC_CAC: // copy boolean acc. into condition

    //TODO: case OPC_TRRD: // take real result dynamic
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
    //TODO: case OPC_TIRD: // take integer result dynamic

    case OPC_TIRS: {
        // Take integer result static.
        // Read word from memory at address in register B.
        // Push it in stack as integer value.
        Word value = machine.mem_load(core.B);
        stack.push_int_value(value);
        break;
    }
    //TODO: case OPC_TFR: // take formal result

    //TODO: case OPC_ADRD: // add real dynamic
    //TODO: case OPC_ADRS: // add real static
    //TODO: case OPC_ADID: // add integer dynamic
    //TODO: case OPC_ADIS: // add integer static
    //TODO: case OPC_ADF: // add formal

    //TODO: case OPC_SURD: // subtract real dynamic
    //TODO: case OPC_SURS: // subtract real static
    //TODO: case OPC_SUID: // subtract integer dynamic
    //TODO: case OPC_SUIS: // subtract integer static
    //TODO: case OPC_SUF: // subtract formal

    //TODO: case OPC_MURD: // multiply real dynamic
    //TODO: case OPC_MURS: // multiply real static
    //TODO: case OPC_MUID: // multiply integer dynamic
    //TODO: case OPC_MUIS: // multiply integer
    //TODO: case OPC_MUF: // static multiply formal

    //TODO: case OPC_DIRD: // divide real dynamic
    //TODO: case OPC_DIRS: // divide real static
    //TODO: case OPC_DIID: // divide integer dynamic
    //TODO: case OPC_DIIS: // divide integer static
    //TODO: case OPC_DIF: // divide formal

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
    //TODO: case OPC_IDI: // integer division
    //TODO: case OPC_TTP: // to the power

    //TODO: case OPC_MOR: // more >
    //TODO: case OPC_LST: // at least ≥
    //TODO: case OPC_EQU: // equal =
    //TODO: case OPC_MST: // at most ≤
    //TODO: case OPC_LES: // less <
    //TODO: case OPC_UQU: // unequal ̸=

    //TODO: case OPC_NON: // non ¬
    //TODO: case OPC_AND: // and ∧
    //TODO: case OPC_OR: // or ∨
    //TODO: case OPC_IMP: // implies →
    //TODO: case OPC_QVL: // equivalent ≡

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
        stack.push_ieee(std::sqrtl(stack.pop_ieee()));
        break;
    }
    case OPC_sin: {
        // Function sin(E) - sine of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        stack.push_ieee(std::sinl(stack.pop_ieee()));
        break;
    }
    case OPC_cos: {
        // Function sin(E) - cosine of the value of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        stack.push_ieee(std::cosl(stack.pop_ieee()));
        break;
    }
    //TODO: case OPC_ln:
    //TODO: case OPC_exp:
    //TODO: case OPC_entier:

    //TODO: case OPC_ST: // store
    //TODO: case OPC_STA: // store also
    //TODO: case OPC_STP: // store procedure value
    //TODO: case OPC_STAP: // store also procedure value

    //TODO: case OPC_SCC: // short circuit
    //TODO: case OPC_RSF: // real arrays storage function frame
    //TODO: case OPC_ISF: // integer arrays storage function frame
    //TODO: case OPC_RVA: // real value array storage function frame
    //TODO: case OPC_IVA: // integer value array storage function frame
    //TODO: case OPC_LAP: // local array positioning
    //TODO: case OPC_VAP: // value array positioning

    case OPC_START:
        // Start of the object program.
        stack.erase();
        break;

    case OPC_STOP:
        // End of the object program.
        return true;

    //TODO: case OPC_TFP: // take formal parameter
    //TODO: case OPC_TAS: // type algol symbol
    //TODO: case OPC_OBC6: // output buffer class 6
    //TODO: case OPC_FLOATER:
    //TODO: case OPC_read:

    case OPC_print: {
        // Print number(s).
        auto arg_count = stack.count();
        if (arg_count == 0) {
            // Emit empty line.
            std::cout << '\n';
        } else {
            for (unsigned i = 0; i < arg_count; i++) {
                auto item = stack.get(i);
                if (item.is_int_value()) {
                    x1_print_integer(std::cout, item.get_int());
                } else if (item.is_real_value()) {
                    x1_print_real(std::cout, item.get_real());
                } else {
                    throw std::runtime_error("Cannot negate address");
                }
                std::cout << '\n';
            }
            stack.erase();
        }
        break;
    }
    //TODO: case OPC_TAB:
    //TODO: case OPC_NLCR:
    //TODO: case OPC_XEEN:
    //TODO: case OPC_SPACE:
    //TODO: case OPC_stop:
    //TODO: case OPC_P21:

    default:
        throw std::runtime_error("Unknown OPC " + std::to_string(opc));
    }
    return false;
}
