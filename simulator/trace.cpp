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

    out << "      Memory " << opname << " [" << std::oct << std::setfill('0') << std::setw(5)
        << addr << "] = ";
    x1_print_word_octal(out, val);
    out << std::endl;

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
        out << "      A = ";
        x1_print_word_octal(out, core.A);
        out << std::endl;
    }
    if (core.B != prev.B) {
        out << "      B = ";
        x1_print_word_octal(out, core.B);
        out << std::endl;
    }
    if (core.C != prev.C) {
        out << "      C = ";
        x1_print_word_octal(out, core.C);
        out << std::endl;
    }
    if (core.S != prev.S) {
        out << "      S = ";
        x1_print_word_octal(out, core.S);
        out << std::endl;
    }

    // Update previous state.
    prev = core;

    // Restore output flags.
    out.flags(save_flags);
}
