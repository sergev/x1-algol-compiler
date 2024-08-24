#include <algorithm>
#include <cmath>
#include <sstream>

#include "encoding.h"
#include "machine.h"
#include "opc.h"

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
        // Expect result on return from procedure.
        stack.push_int_value(0); // place for result
        frame_create(OT, core.A);
        OT = core.B;
        break;

    case OPC_ETMP:
        // extransmark procedure
        // Invoke a procedure which address is located in register B.
        // Number of arguments is present in register A.
        // Save return address on stack.
        // Note: descriptors of procedure arguments are located
        // in memory 3 words before the return address.
        machine.mem_store(51, OT - 8); // for PRINTTEXT
        stack.push_null();             // place for result
        frame_create(OT, core.A);
        OT = core.B;
        break;

        // TODO: case OPC_FTMR: // formtransmark result
        // TODO: case OPC_FTMP: // formtransmark procedure

    case OPC_FOR8:
        // Drop the unneeded address of the controlled variable.
        stack.pop();
        // The loop subroutine returns nothing.
        stack.set_null(frame_ptr + Frame_Offset::RESULT);

        // Return from the loop subroutine using the provided destination.
        stack.set_int_addr(frame_ptr + Frame_Offset::PC, core.S);
        // In the complex, FOR8 jumps to RET.
        // FALLTHRU

    case OPC_RET: {
        // return from procedure
        // Jump to address from stack.
        OT = frame_release();
        if (stack.top().is_null()) {
            // Drop empty result.
            stack.pop();
        }
        break;
    }
    case OPC_EIS: {
        // end of implicit subroutine
        auto item   = stack.pop();
        OT          = frame_release();
        stack.top() = item;
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
        if (need_formal_address) {
            throw std::runtime_error("Illegal argument expression");
        }
        need_formal_address = true;
        push_formal_value(core.S);
        need_formal_address = false;
        break;
    }

    case OPC_FOR1:
        // Save the execution link (which points to a jump to the loop body)
        // in the local variable.
        stack.set_int_addr(frame_ptr + Frame_Offset::ARG, OT);
        // When starting to execute a new loop element,
        // jump to it (initially PC points to under the ETMP for the loop,
        // then it gets advanced by the OPCs of the loop elements
        // as they get exhausted).
        OT = stack.get(frame_ptr + Frame_Offset::PC).get_addr();
        break;

    case OPC_FOR2: {
        // The element does not repeat.
        stack.set_int_addr(frame_ptr + Frame_Offset::PC, OT);
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
        stack.push(dest); // for the next element in case of exhaustion
        break;
    }

    case OPC_FOR4:
        // Check the while condition.
        core.C = stack.pop_boolean();
        if (core.C) {
            stack.pop(); // address pushed by FOR3 not needed
            // Go to the loop iteration.
            OT = stack.get(frame_ptr + Frame_Offset::ARG).get_addr();
        } else {
            // Indicate end of the 'while' element, continue to the next one.
            stack.set_int_addr(frame_ptr + Frame_Offset::PC, OT);
        }
        break;

    case OPC_FOR5:
        // Indicate entering the step-until element.
        stack.set_null(frame_ptr + Frame_Offset::RESULT);
        // The job of FOR5 is done.
        stack.set_int_addr(frame_ptr + Frame_Offset::PC, OT);
        break;

    case OPC_FOR6: {
        auto incr    = stack.pop();
        int step_dir = incr.sign();
        if (stack.get(frame_ptr + Frame_Offset::RESULT).is_null()) {
            // First iteration: the actual value of the step is ignored,
            // we're only interested in its sign.
            // The initial value is now on top of the stack, pending FOR7 check.
        } else {
            // A subsequent iteration: load and increment the controlled variable.
            auto var   = stack.pop();
            auto value = load_value(var);
            value.add(incr);
            // Prepare the stack for storing the new value of the variable.
            stack.push(var);
            stack.push(value);
        }
        stack.set_int_value(frame_ptr + Frame_Offset::RESULT, integer_to_x1(step_dir));
        break;
    }

    case OPC_FOR7: {
        auto limit = stack.pop();
        auto value = stack.pop();
        auto var   = stack.pop();
        store_value(var, value);
        int step_dir = x1_to_integer(stack.get(frame_ptr + Frame_Offset::RESULT).get_int());
        if ((step_dir > 0 && limit.is_less(value)) || (step_dir < 0 && value.is_less(limit))) {
            // Element exhausted.
            stack.push(var); // For the sake of the next loop element.
            stack.set_int_addr(frame_ptr + Frame_Offset::PC, OT);
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
    case OPC_SUF: {
        // subtract formal
        // Left argument is on stack.
        // Register S has fynamic address of right argument.
        push_formal_value(core.S);
        auto b = stack.pop();
        stack.top().subtract(b);
        break;
    }

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
    case OPC_MUF: {
        // multiply formal
        // Left argument is on stack.
        // Register S has fynamic address of right argument.
        push_formal_value(core.S);
        auto b = stack.pop();
        stack.top().multiply(b);
        break;
    }

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
    case OPC_DIF: {
        // divide formal
        // Left argument is on stack.
        // Register S has fynamic address of right argument.
        push_formal_value(core.S);
        auto b = stack.pop();
        stack.top().divide(b);
        break;
    }

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
        int location  = x1_to_integer(load_word(addr));
        int base      = load_word(addr + 1);
        int limit     = x1_to_integer(load_word(addr + 2 + ndim));
        // The limit must look like a negative number with a reasonable absolute value.
        if (limit >= 0 || -limit > 32767) {
            throw std::runtime_error("A wrong number of indexes for an array");
        }
        int elt_addr = base;
        int idx0     = 0; // for the error message
        for (unsigned i = 0; i < ndim; ++i) {
            int dimsize = x1_to_integer(load_word(addr + 2 + i));
            int idx     = idxs[i].is_int_value() ? x1_to_integer(idxs[i].get_int())
                                                 : (int)roundl(x1_to_ieee(idxs[i].get_real()));
            if (i == 0)
                idx0 = idx;
            elt_addr += idx * dimsize;
        }
        elt_addr &= BITS(16); // 16 rather than 15 is to catch "negative" addresses
        if (elt_addr < location || elt_addr + limit >= location) {
            std::ostringstream ostr;
            int elsize = x1_to_integer(load_word(addr + 2));
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
        auto src = stack.pop();
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
        auto item   = stack.pop();
        Word result = integer_to_x1(item.sign());
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
        auto addr   = get_display(core.B + 1) + Frame_Offset::RESULT;
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
        auto addr          = get_display(core.B + 1) + Frame_Offset::RESULT;
        if (!stack.get(addr).is_null()) {
            stack.set(addr, result);
        }
        break;
    }

    case OPC_FOR0: {
        // Enters an implicit subroutine with one local word.
        // In the complex, B := BN + 1; where BN is M[52]
        auto prev_fp = stack.get(frame_ptr + Frame_Offset::FP).get_addr();
        auto prev_bn = stack.get(prev_fp + Frame_Offset::BN).get_int();
        core.B       = prev_bn + 1;
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
        set_block_level(core.B);
        update_display(core.B, frame_ptr);
        break;
    }
    case OPC_RSF: {
        // real arrays storage function frame
        make_storage_function_frame(2);
        break;
    }
    case OPC_ISF: {
        // integer arrays storage function frame
        make_storage_function_frame(1);
        break;
    }
        // TODO: case OPC_RVA: // real value array storage function frame
        // TODO: case OPC_IVA: // integer value array storage function frame
    case OPC_LAP: {
        // local array positioning
        unsigned addr = address_in_stack(core.S);
        if (stack.get(addr).get_addr() != 0) {
            throw std::runtime_error("Misplaced array storage function frame");
        }
        Cell_Type ct = stack.get(addr).type;
        int pos;
        // Find the first negative number in the storage function.
        // It is the negated array length.
        for (pos = 2; pos < 32; ++pos)
            if (stack.get(addr + pos).is_int_value() &&
                x1_to_integer(stack.get(addr + pos).get_int()) < 0)
                break;
        if (pos == 32)
            throw std::runtime_error("Runaway array storage function frame");
        unsigned words = -x1_to_integer(stack.get(addr + pos).get_int());
        stack.set(addr, Stack_Cell{ ct, stack_base + STACK_BASE });
        int offset = x1_to_integer(stack.get(addr + 1).get_int());
        stack.set(addr + 1, Stack_Cell{ ct, stack_base - offset + STACK_BASE });
        allocate_stack(words);
        stack_base += words;
        break;
    }
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
