#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "machine.h"

//
// Flag to enable tracing.
//
bool Machine::debug_flag;

//
// Emit trace to this stream.
//
std::ofstream Machine::trace_stream;

//
// Redirect trace output to a given file.
//
void Machine::redirect_trace(const char *file_name, bool on)
{
    if (trace_stream.is_open()) {
        // Close previous file.
        trace_stream.close();
    }
    if (file_name && file_name[0]) {
        // Open new trace file.
        trace_stream.open(file_name);
        if (!trace_stream.is_open())
            throw std::runtime_error("Cannot write to " + std::string(file_name));
    }

    if (!trace_enabled()) {
        enable_trace(on);
    }
}

std::ostream &Machine::get_trace_stream()
{
    if (trace_stream.is_open()) {
        return trace_stream;
    }
    return std::cout;
}

void Machine::close_trace()
{
    if (trace_stream.is_open()) {
        // Close output.
        trace_stream.close();
    }

    // Disable trace options.
    enable_trace(false);
}

//
// Trace output
//
void Machine::print_exception(const char *message)
{
    auto &out = Machine::get_trace_stream();
    out << "--- " << message << std::endl;
}

//
// Print memory read/write.
//
void Machine::print_memory_access(unsigned addr, Word val, const char *opname)
{
    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << "       Memory " << opname << " [" << std::oct << std::setfill('0') << std::setw(5)
        << addr << "] = ";
    x1_print_word_octal(out, val);
    out << std::endl;

    // Restore.
    out.flags(save_flags);
}

//
// Print stack push/pop.
//
void Machine::print_stack_op(unsigned offset, const std::string &value, const std::string &opname)
{
    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << "       Stack " << opname << " [" << std::oct << offset << "] = " << value << std::endl;

    // Restore.
    out.flags(save_flags);
}

//
// Print display[] change.
//
void Machine::print_display(unsigned level, unsigned value)
{
    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << "       Display [" << std::oct << level << "] = ";
    out << std::setfill('0') << std::setw(5) << value;
    out << std::endl;

    // Restore.
    out.flags(save_flags);
}

//
// Print change of lexical level.
//
void Machine::print_level(unsigned new_level, unsigned new_frame)
{
    unsigned old_level = cpu.get_block_level();
    unsigned old_frame = cpu.get_display(old_level);

    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << "       Change Level " << std::oct << old_level << ", Frame " << old_frame
        << " -> Level " << new_level << ", Frame " << new_frame
        << std::endl;

    // Restore.
    out.flags(save_flags);
}

void Machine::print_level()
{
    unsigned old_level = cpu.get_block_level();
    unsigned old_frame = cpu.get_display(old_level);

    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << "       Restore Level " << std::oct << old_level << ", Frame " << old_frame
        << std::endl;

    // Restore.
    out.flags(save_flags);
}

//
// Print instruction address, opcode from OR and mnemonics.
//
void Processor::print_instruction()
{
    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    out << std::oct << std::setfill('0') << std::setw(5) << OT << ": ";
    x1_print_word_octal(out, OR);
    out << ' ';
    x1_print_instruction(out, OR);
    out << std::endl;

    // Restore.
    out.flags(save_flags);
}

//
// Print changes in CPU registers.
//
void Processor::print_registers()
{
    auto &out       = Machine::get_trace_stream();
    auto save_flags = out.flags();

    if (core.A != prev.A) {
        out << "       A = ";
        x1_print_word_octal(out, core.A);
        out << std::endl;
    }
    if (core.B != prev.B) {
        out << "       B = " << std::oct << core.B << std::dec << std::endl;
    }
    if (core.C != prev.C) {
        out << "       C = " << core.C << std::endl;
    }
    if (core.S != prev.S) {
        out << "       S = ";
        x1_print_word_octal(out, core.S);
        out << std::endl;
    }
    if (stack.count() != prev_stack_ptr) {
        out << "       SP = " << std::oct << stack.count() << std::dec << std::endl;
    }
    if (frame_ptr != prev_frame_ptr) {
        out << "       FP = " << std::oct << frame_ptr << std::dec << std::endl;
    }

    // Update previous state.
    prev = core;
    prev_frame_ptr = frame_ptr;
    prev_stack_ptr = stack.count();

    // Restore output flags.
    out.flags(save_flags);
}
