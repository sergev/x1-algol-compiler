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
        machine.mem_store(51, OT - 8); // for PRINTTEXT
        stack.push_null();             // place for result
        frame_create(OT, core.A);
        OT = core.B;
        break;

    case OPC_FTMR: {
        // formtransmark result
        // Invoke a function which dynamic address is located in register S.
        // Number of arguments is present in register A.
        // Expect result on return from procedure.
        unsigned addr = get_formal_proc(core.S);
        stack.push_int_value(0); // place for result
        frame_create(OT, core.A);
        OT = addr;
        break;
    }
    case OPC_FTMP: {
        // formtransmark procedure
        // Invoke a procedure which dynamic address is located in register S.
        // Number of arguments is present in register A.
        // Save return address on stack.
        unsigned addr = get_formal_proc(core.S);
        machine.mem_store(51, OT - 8); // for PRINTTEXT
        stack.push_null();             // place for result
        frame_create(OT, core.A);
        OT = addr;
        break;
    }
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
        auto post_op = eis_operation.at(stack_base);
        OT           = frame_release();
        if (stack.top().is_null()) {
            // Drop empty result.
            stack.pop();
        } else {
            // Apply operation on top of stack.
            apply_operation(post_op);
        }
        break;
    }
    case OPC_EIS: {
        // end of implicit subroutine
        auto item    = stack.pop();
        auto post_op = eis_operation.at(stack_base);
        if (post_op == Formal_Op::REMOVE_ARG) {
            stack_base -= 2;
        }
        frame_ptr = stack_base + Frame_Offset::FP - Frame_Offset::ARG;
        OT        = frame_release();

        // Apply operation with given item.
        stack.top() = item;
        apply_operation(post_op);
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
        take_formal(core.S, Formal_Op::PUSH_ADDRESS);
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

    case OPC_GTA:
        // goto adjustment
        // Exit to block level given in register B.
        frame_ptr = last_frame_ptr;
        roll_back(display[core.B]);
        break;

    case OPC_SSI: {
        // store switch index
        auto index = stack.pop().get_int();
        machine.mem_store(060, index);
        break;
    }
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
        take_formal(core.S, Formal_Op::PUSH_VALUE);
        break;
    case OPC_ADRD: {
        // add real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().add(b);
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        // Register S has dynamic address of right argument.
        take_formal(core.S, Formal_Op::ADD);
        break;
    }
    case OPC_SURD: {
        // subtract real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().subtract(b);
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        // Register S has dynamic address of right argument.
        take_formal(core.S, Formal_Op::SUBTRACT);
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
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
        break;
    }
    case OPC_MUIS: {
        // multiply integer static
        Stack_Cell b;
        b.value = machine.mem_load(core.B);
        b.type = Cell_Type::INTEGER_VALUE;
        stack.top().multiply(b);
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
        break;
    }
    case OPC_MUF: {
        // multiply formal
        // Left argument is on stack.
        // Register S has dynamic address of right argument.
        take_formal(core.S, Formal_Op::MULTIPLY);
        break;
    }

    case OPC_DIRD: {
        // divide real dynamic
        auto b = get_dynamic_real(core.S);
        stack.top().divide(b);
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
        // Register S has dynamic address of right argument.
        take_formal(core.S, Formal_Op::DIVIDE);
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
        if (limit >= 0 || -limit > 0xfffff) {
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
        elt_addr &= BITS(20); // 16 rather than 15 is to catch "negative" addresses
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
        Machine::trace_stack(stack.count(), stack.top().to_string(), "Write");
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
    case OPC_arctan: {
        // Function arctan(E) - arc tangent function of E
        // Yield value of type real.
        // Argument can be either of type real or integer.
        stack.push_ieee(atanl(stack.pop_ieee()));
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
        increment_stack_base(1);
        // In the complex, FOR0 jumps to SCC.
    }
        // FALLTHRU
    case OPC_SCC: {
        // short circuit
        // Numeric argument is present in register B.
        if (core.B > 31) {
            throw std::runtime_error("Bad block level in SCC");
        }
        if (get_block_level() != 0) {
            // Convert implicit subroutine into procedure.
            frame_ptr = stack_base + Frame_Offset::FP - Frame_Offset::ARG;
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
    case OPC_RVA: {
        // real value array storage function frame
        make_value_array_function_frame(2);
        break;
    }
    case OPC_IVA: {
        // integer value array storage function frame
        make_value_array_function_frame(1);
        break;
    }
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
        increment_stack_base(words);
        break;
    }
    case OPC_VAP: {
        // value array positioning
        unsigned storage_fn = address_in_stack(core.S);
        unsigned actual     = stack.get(storage_fn).get_addr();
        unsigned zero_base =
            stack.get(storage_fn + 1).get_addr() & 0xFFFF; // remove stray high bits
        unsigned elt_size = stack.get(storage_fn + 2).get_int();
        Cell_Type ct = elt_size % 2 == 1 ? Cell_Type::INTEGER_ADDRESS : Cell_Type::REAL_ADDRESS;
        // Element size correctors for type conversion.
        int mul = 1, div = 1;
        switch (elt_size) {
        case 21: // real to integer
            div = 2;
            break;
        case 12: // integer to real
            mul = 2;
            break;
        case 1:
        case 2:
            break;
        default:
            throw std::runtime_error("Impossible type conversion in VAP");
        }
        int pos;
        // Find the first negative number in the storage function.
        // It is the negated array length.
        for (pos = 2; pos < 8; ++pos)
            if (stack.get(storage_fn + pos).is_int_value() &&
                x1_to_integer(stack.get(storage_fn + pos).get_int()) < 0)
                break;
        if (pos == 8)
            throw std::runtime_error("Runaway value array storage function frame");
        if (mul != 1 || div != 1) {
            // Correcting dimension sizes and the word count.
            for (int i = 3; i <= pos; ++i) {
                int value = x1_to_integer(stack.get(storage_fn + i).get_int());
                stack.set_int_value(storage_fn + i, integer_to_x1(value * mul / div));
            }
        }
        unsigned words = -x1_to_integer(stack.get(storage_fn + pos).get_int());
        stack.set(storage_fn, Stack_Cell{ ct, stack_base + STACK_BASE });
        int offset = actual - zero_base;
        offset     = offset * mul / div;
        stack.set(storage_fn + 1, Stack_Cell{ ct, stack_base - offset + STACK_BASE });
        for (unsigned i = 0; i < words; ++i) {
            switch (elt_size) {
            case 1:
                stack.push_int_value(load_word(actual + i));
                break;
            case 2:
                stack.push_real_value(load_real(actual + i));
                break;
            case 21: // real to integer
                stack.push_int_value(integer_to_x1(roundl(x1_to_ieee(load_real(actual + 2 * i)))));
                break;
            case 12: // integer to real
                stack.push_real_value(
                    ieee_to_x1((long double)x1_to_integer(load_word(actual + i / 2))));
                break;
            }
        }
        if (elt_size > 2) {
            // Finally, store the true element size.
            stack.set_int_value(storage_fn + 2, elt_size % 10);
        }
        increment_stack_base(words);
        break;
    }
    case OPC_START:
        // Start of the object program.
        break;

    case OPC_STOP:
        // End of the object program.
        return true;

    case OPC_TFP: {
        // take formal parameter
        auto block_level = get_block_level();
        if (block_level == 0) {
            // Allocate a block level.
            auto prev_fp = stack.get(frame_ptr + Frame_Offset::FP).get_addr();
            auto prev_bn = stack.get(prev_fp + Frame_Offset::BN).get_int();
            block_level  = prev_bn + 1;
            set_block_level(block_level);
            update_display(block_level, frame_ptr);
        }
        // Get argument.
        eis_operation[stack_base] = Formal_Op::REMOVE_ARG;
        take_formal((Frame_Offset::ARG << 5) + block_level, Formal_Op::PUSH_VALUE);
        break;
    }

    case OPC_TAS:
        // type Algol symbol
        algol_putc(core.S, std::cout);
        std::cout << std::flush;
        break;
        // TODO: case OPC_OBC6: // output buffer class 6
        // TODO: case OPC_FLOATER:
    case OPC_read:
        // Read real number from stdin.
        stack.push_ieee(Machine::input_real(std::cin));
        break;

    case OPC_print: {
        // Print number(s).
#if 1
        // Modern version: any number of arguments.
        for (auto i = stack_base; i < stack.count(); i++) {
            if (i > stack_base) {
                // Put space between arguments.
                std::cout << ' ';
            }
            auto item = stack.get(i);
            if (item.is_int_value()) {
                x1_print_integer(std::cout, item.get_int());
            } else if (item.is_real_value()) {
                x1_print_real(std::cout, item.get_real());
            } else {
                throw std::runtime_error("Cannot print address");
            }
        }
        stack.erase(stack_base);
#else
        // Authentic version, as in X8 Algol.
        if (stack.count() != stack_base + 1) {
            throw std::runtime_error("Wrong argument for print()");
        }
        auto x = stack.pop_ieee();
        Machine::print_int_or_real(std::cout, x);
#endif
        std::cout << std::flush;
        break;
    }

    case OPC_TAB:
        std::cout << '\t';
        break;

    case OPC_NLCR:
        std::cout << std::endl;
        break;

    case OPC_XEEN: {
        // Read console switches.
        auto bitmask = stack.pop_integer();
        stack.push_int_value(machine.read_console_switches(std::cin, bitmask));
        break;
    }
    case OPC_SPACE: {
        // Print N spaces.
        auto a = stack.pop_integer();
        while (a-- > 0) {
            std::cout << ' ';
        }
        std::cout << std::flush;
        break;
    }
    case OPC_stop:
        // Stop machine and ask operator for console input.
        machine.ask_console_input();
        break;

        // TODO: case OPC_P21:

    case OPC_FLOT: {
        // Print number in floating-point representation.
        auto x = stack.pop_ieee();
        int m;
        if (stack.count() == stack_base + 1) {
            // Invoked with two arguments: FLOT(n, x);
            m = 2;
        } else if (stack.count() == stack_base + 2) {
            // Invoked with three arguments: FLOT(n, m, x);
            m = stack.pop_integer();
        } else {
            throw std::runtime_error("Wrong arguments for FLOT()");
        }
        auto n = stack.pop_integer();
        Machine::print_floating_point(std::cout, n, m, x);
        std::cout << std::flush;
        break;
    }
    case OPC_FIXT: {
        // Print number in fixed-point representation, with sign.
        auto x = stack.pop_ieee();
        auto m = stack.pop_integer();
        auto n = stack.pop_integer();
        Machine::print_fixed_point(std::cout, n, m, x, true);
        std::cout << std::flush;
        break;
    }
    case OPC_ABSFIXT: {
        // Print absolute value in fixed-point representation, no sign.
        auto x = stack.pop_ieee();
        auto m = stack.pop_integer();
        auto n = stack.pop_integer();
        Machine::print_fixed_point(std::cout, n, m, x, false);
        std::cout << std::flush;
        break;
    }

    default: {
        std::ostringstream ostr;
        x1_print_instruction(ostr, opc);
        throw std::runtime_error("NYI OPC " + ostr.str());
    }
    }
    return false;
}
