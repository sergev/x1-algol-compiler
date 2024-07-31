#ifndef X1_MACHINE_H
#define X1_MACHINE_H

#include <array>
#include <chrono>
#include <memory>
#include <vector>

#include "processor.h"

class Machine {
private:
    // Enable a progress message to stderr.
    bool progress_message_enabled{ false };

    // Every few seconds, print a message to stderr, to track the simulation progress.
    void show_progress();

    // Time of last check.
    std::chrono::time_point<std::chrono::steady_clock> progress_time_last{ std::chrono::steady_clock::now() };

    // Last instr_count when progress message was printed.
    uint64_t progress_count{ 0 };

    // Trace output.
    static std::ofstream trace_stream;

    // Trace machine instuctions and registers.
    static bool debug_flag;

    // Count of instructions.
    static uint64_t simulated_instructions;

    // A name of the file to run.
    std::string input_file;

    // Status of the simulation.
    int exit_status{ EXIT_SUCCESS };

public:
    // 32K words of memory.
    std::array<Word, MEMORY_NWORDS> memory{};

    // Electrologica X1 processor.
    Processor cpu{ *this };

    // Destructor.
    ~Machine();

    // Set name of input file.
    void set_input_file(const char *filename);
    void set_input_file(const std::string &filename) { set_input_file(filename.c_str()); }
    const std::string &get_input_file() const { return input_file; }

    // Load Algol input into machine.
    void load_algol(const std::string &filename);
    void load_algol(std::istream &input);

    // Run simulation.
    void run();

    // Get status of simulation: either EXIT_SUCCESS (0) or
    // EXIT_FAILURE in case of errors.
    int get_exit_status() const { return exit_status; }

    // Enable a progress message to stderr.
    void enable_progress_message(bool on) { progress_message_enabled = on; }

    // Get instruction count.
    static uint64_t get_instr_count() { return simulated_instructions; }
    static void incr_simulated_instructions() { simulated_instructions++; }

    // Enable trace output to the given file,
    // or to std::cout when filename not present.
    static void enable_trace(bool on = true) { debug_flag = on; }
    static void redirect_trace(const char *file_name, bool on = debug_flag);
    static void close_trace();
    static bool trace_enabled() { return debug_flag; }

    // Emit trace to this stream.
    static std::ostream &get_trace_stream();

    // Memory access.
    Word mem_fetch(unsigned addr);
    Word mem_load(unsigned addr);
    void mem_store(unsigned addr, Word val);

    //
    // Trace methods.
    //
    static void trace_exception(const char *message)
    {
        if (trace_enabled())
            print_exception(message);
    }

    static void trace_fetch(unsigned addr, Word val)
    {
        if (debug_flag)
            print_fetch(addr, val);
    }

    static void trace_memory_write(unsigned addr, Word val)
    {
        if (debug_flag)
            print_memory_access(addr, val, "Write");
    }

    static void trace_memory_read(unsigned addr, Word val)
    {
        if (debug_flag)
            print_memory_access(addr, val, "Read");
    }

    void trace_instruction(unsigned opcode)
    {
        if (debug_flag)
            cpu.print_instruction();
    }

    void trace_registers()
    {
        if (debug_flag)
            cpu.print_registers();
    }

    static void print_exception(const char *message);
    static void print_fetch(unsigned addr, Word val);
    static void print_memory_access(unsigned addr, Word val, const char *opname);
};

//
// Decode word as string.
//
std::string word_string(Word w);

#endif // X1_MACHINE_H
