#include "machine.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

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

//
// Set name of compiler.
// For now it has to be a full path.
//
void Machine::set_compiler(const std::string &filename)
{
    compiler_path = filename;
}

//
// Compile Algol file to object format.
//
void Machine::compile(const std::string &algol_filename, const std::string &obj_filename)
{
    run_program(compiler_path, algol_filename, obj_filename);
}

//
// Run external program with given input and output files.
// Throw exception in case of error.
//
void Machine::run_program(const std::string &prog_path, const std::string &input_filename,
                          const std::string &output_filename)
{
    // A child process passes this status code to the parent.
    enum {
        STATUS_OK,
        STATUS_CANNOT_READ_INPUT,
        STATUS_CANNOT_WRITE_OUTPUT,
        STATUS_CANNOT_RUN_PROGRAM,
    };

    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Cannot fork");
    }

    if (pid == 0) {
        //
        // Child process.
        //

        // Open input file.
        int in_fd = open(input_filename.c_str(), O_RDONLY);
        if (in_fd < 0) {
            exit(STATUS_CANNOT_READ_INPUT);
        }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);

        // Create output file.
        int out_fd = open(output_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            exit(STATUS_CANNOT_WRITE_OUTPUT);
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);

        // Start compiler.
        execlp(prog_path.c_str(), prog_path.c_str(), nullptr);
        exit(STATUS_CANNOT_RUN_PROGRAM);
    }

    //
    // Parent process.
    //
    int wait_status;
    if (waitpid(pid, &wait_status, 0) < 0) {
        throw std::runtime_error("Lost child process #" + std::to_string(pid));
    }

    int exit_code = WEXITSTATUS(wait_status);
    switch (exit_code) {
    case STATUS_OK:
        return;
    case STATUS_CANNOT_READ_INPUT:
        throw std::runtime_error("Cannot read " + input_filename);
    case STATUS_CANNOT_WRITE_OUTPUT:
        throw std::runtime_error("Cannot write " + output_filename);
    case STATUS_CANNOT_RUN_PROGRAM:
        throw std::runtime_error("Cannot execute " + prog_path);
    default:
        throw std::runtime_error("Compiler failed with status " + std::to_string(exit_code));
    }
}

//
// Load object file into memory.
//
void Machine::load_object_program(const std::string &obj_filename)
{
    std::ifstream input(obj_filename);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot read " + obj_filename);
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line[0] == 0) {
            // Ignore empty lines.
            continue;
        }

        char name[16];
        unsigned addr, value, a, b, c;
        if (std::sscanf(line.c_str(), "%9s %u %u %u", name, &a, &b, &c) == 4) {
            //
            // Symbol definition, for example:
            // "A       00 00 02"
            //
            addr = (a * 32 + b) * 32 + c;
            symbol_table[name] = addr;

        } else if (std::sscanf(line.c_str(), "%u %u %u", &a, &b, &c) == 3) {
            //
            // Entry address, for example:
            // " 9 26  0"
            //
            addr = (a * 32 + b) * 32 + c;
            entry_table.push_back(addr);

        } else if (std::sscanf(line.c_str(), "%u %u", &addr, &value) == 2) {
            //
            // One word of object code, for example:
            // "10048       96"
            //
            mem_store(addr, value);
        } else {
            throw std::runtime_error("Bad line: '" + line + "'");
        }
    }
}

#if 0
//
// Decode word to UTF-8 string.
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

//
// Print string from memory.
//
void Machine::print_string(std::ostream &out, unsigned addr)
{
    BytePointer bp(memory, ADDR(addr));
    for (;;) {
        auto ch = bp.get_byte();
        if (ch == '\0')
            break;
        algol_putc(ch, out);
    }
    out << '\n';
}
#endif
