#include "machine.h"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

//TODO: #include "encoding.h"

// Static fields.
uint64_t Machine::simulated_instructions = 0;

//
// Deallocate the machine: disable tracing.
//
Machine::~Machine()
{
    redirect_trace(nullptr, false);
}

//
// Set name of input file.
//
void Machine::set_input_file(const char *filename)
{
    if (!input_file.empty()) {
        std::cerr << "Too many input files: " << filename << std::endl;
        ::exit(EXIT_FAILURE);
    }
    input_file = filename;
}

//
// Every few seconds, print a message to stderr, to track the simulation progress.
//
void Machine::show_progress()
{
    //
    // Check the real time every few thousand cycles.
    //
    static const uint64_t PROGRESS_INCREMENT = 10000;

    if (simulated_instructions >= progress_count + PROGRESS_INCREMENT) {
        progress_count += PROGRESS_INCREMENT;

        // How much time has passed since the last check?
        auto time_now = std::chrono::steady_clock::now();
        auto delta    = time_now - progress_time_last;
        auto sec      = std::chrono::duration_cast<std::chrono::seconds>(delta).count();

        // Emit message every 5 seconds.
        if (sec >= 5) {
            std::cerr << "----- Progress " << simulated_instructions << " -----" << std::endl;
            progress_time_last = time_now;
        }
    }
}

//
// Run the machine until completion.
//
void Machine::run()
{
    // Show initial state.
    trace_registers();

    try {
        for (;;) {
            bool done = cpu.step();

            // Show changed registers.
            trace_registers();

            if (progress_message_enabled) {
                show_progress();
            }
            simulated_instructions++;

            if (done) {
                // Halted by 'стоп' instruction.
                return;
            }
        }
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit_status = EXIT_FAILURE;
    }
}

//
// Fetch instruction word.
//
Word Machine::mem_fetch(unsigned addr)
{
    Word val = memory[addr];

    trace_fetch(addr, val);
    return val & BITS(27);
}

//
// Write word to memory.
//
void Machine::mem_store(unsigned addr, Word val)
{
    addr &= BITS(15);

    memory[addr] = val;
    trace_memory_write(addr, val);
}

//
// Read word from memory.
//
Word Machine::mem_load(unsigned addr)
{
    addr &= BITS(15);

    Word val = memory[addr];
    trace_memory_read(addr, val);

    return val & BITS(27);
}

#if 0
//
// Load algol source file.
// Throw exception on failure.
//
void Machine::load_algol(const std::string &filename)
{
    if (trace_enabled()) {
        std::cout << "Read '" << filename << "'\n";
    }

    // Open the input file.
    std::ifstream input;
    input.open(filename);
    if (!input.is_open())
        throw std::runtime_error(filename + ": " + std::strerror(errno));

    load_algol(input);
}

//
// Load algol source from stream.
//
void Machine::load_algol(std::istream &input)
{
    while (input.good()) {
        // Get next line.
        std::string line;
        getline(input, line);
        //TODO
    }
}

//
// Decode word to ASCII string.
//
std::string word_string(Word w)
{
    std::ostringstream buf;
    for (int shift = 0; shift < 24; shift += 8) {
        uint8_t ch = w >> shift;
        algol_putc(ch, buf);
    }
    return buf.str();
}

                   [](unsigned char c) { return std::tolower(c); });
    return filename;
}

//
// Send message to operator's console.
//
void Machine::print_string(std::ostream &out, unsigned addr)
{
    BytePointer bp(memory, ADDR(addr));
    for (;;) {
        auto ch = bp.get_byte();
        if (ch == '\0')
            break;
        iso_putc(ch, out);
    }
    out << '\n';
}
#endif
