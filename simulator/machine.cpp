#include "machine.h"

#include <fcntl.h>
#include <sys/wait.h>
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

#include "byte_pointer.h"
#include "encoding.h"

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
// Compile the input Algol file and simulate it.
//
void Machine::compile_and_run()
{
    const auto input_path = std::filesystem::path(input_file);
    const auto extension  = input_path.extension().string();

    if (extension == ".a60") {
        //
        // Compile file.a60 to file.x1, then load.
        //
        const auto obj_filename = input_path.stem().string() + ".x1";
        compile(input_file, obj_filename);
        load_object_program(obj_filename);

    } else if (extension == ".x1") {
        //
        // Load binary file.x1.
        //
        load_object_program(input_file);
    } else {
        throw std::runtime_error("Unknown file extension: " + extension);
    }
    run(get_entry(0));
}

//
// Run the machine until completion, or until PC address reached.
//
void Machine::run(unsigned start_addr, unsigned finish_addr, unsigned finish_frame)
{
    // Jump to the first entry.
    cpu.set_ot(start_addr);

    // Show initial state.
    trace_registers();

    auto const frame_base = cpu.get_frame_ptr();
    for (;;) {
        bool done{};
        try {
            done = cpu.step();

        } catch (const Non_Local_Goto&) {
            // Non-local GOTO: check whether this machine is the target.
            if (cpu.get_frame_ptr() < frame_base) {
                // Send to previous level.
                throw;
            }
        }

        // Show changed registers.
        trace_registers();

        if (progress_message_enabled) {
            show_progress();
        }
        simulated_instructions++;

        if (done) {
            // Halted by 'STOP' code.
            return;
        }
        if (finish_addr != 0 && cpu.at_address(finish_addr, finish_frame)) {
            // End of implicit subroutine.
            return;
        }
    }
}

//
// Fetch instruction word.
//
Word Machine::mem_fetch(unsigned addr)
{
    Word val = memory[addr];

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
    if (compiler_path.empty()) {
        throw std::runtime_error("Compiler is not selected");
    }
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
        STATUS_OK              = EXIT_SUCCESS,
        STATUS_COMPILER_FAILED = EXIT_FAILURE,
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
    case STATUS_COMPILER_FAILED:
        throw std::runtime_error("Fatal error in Algol compiler");
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
            addr               = (a * 32 + b) * 32 + c;
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
