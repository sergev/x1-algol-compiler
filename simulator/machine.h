#ifndef X1_MACHINE_H
#define X1_MACHINE_H

#include <array>
#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "processor.h"

class Machine {
private:
    // Enable a progress message to stderr.
    bool progress_message_enabled{ false };

    // Every few seconds, print a message to stderr, to track the simulation progress.
    void show_progress();

    // Time of last check.
    std::chrono::time_point<std::chrono::steady_clock> progress_time_last{
        std::chrono::steady_clock::now()
    };

    // Last instr_count when progress message was printed.
    uint64_t progress_count{ 0 };

    // Trace output.
    static std::ofstream trace_stream;

    // Trace machine instuctions and registers.
    static bool debug_flag;

    // Count of instructions.
    static uint64_t simulated_instructions;

    // Provided input files (Algol source, X1 object file, libraries)
    std::vector<std::string> input_files;

    // Compiler of choice.
    std::string compiler_path;

    // Status of the simulation.
    int exit_status{ EXIT_SUCCESS };

    // 32K words of memory.
    std::array<Word, MEMORY_NWORDS> memory{};

    // Table of symbol addresses from object program.
    std::map<std::string, unsigned> symbol_table;

    // Table of entry addresses from object program.
    std::vector<unsigned> entry_table;

    // True when stdin is connected to terminal.
    static bool is_interactive;

    // State of console switches.
    unsigned console_switches{};
    bool switches_are_valid{};

    // Initial value for random number generator.
    unsigned random_seed{};
    Real random_output{};
    std::mt19937 random_generator{ 1 };

public:
    // Electrologica X1 processor.
    Processor cpu{ *this };

    // Destructor.
    ~Machine();

    // Add a file to the list of input files.
    void add_input_file(const std::string &filename);

    // Return the first input file (Algol source or X1 object file).
    const std::string &get_input_file() const;

    // Set name of compiler.
    void set_compiler(const std::string &filename);

    // Compile Algol file to object format.
    void compile(const std::vector<std::string> &filenames, const std::string &obj_filename);

    // Load object file into memory.
    void load_object_program(const std::string &obj_filename);

    // Run external program with given input and output files.
    void run_program(const std::string &prog_path, const std::vector<std::string> &input_filenames,
                     const std::string &output_filename);

    // Compile the input Algol file and simulate it.
    void compile_and_run();

    // Run simulation.
    void run(unsigned start_addr);

    // Get address by name from symbol table.
    unsigned get_symbol(const std::string &name) { return symbol_table.at(name); }

    // Get entry address by index.
    unsigned get_entry(unsigned index) { return entry_table[index]; }

    // Get status of simulation: either EXIT_SUCCESS (0) or
    // EXIT_FAILURE in case of errors.
    int get_exit_status() const { return exit_status; }

    // Enable a progress message to stderr.
    void enable_progress_message(bool on) { progress_message_enabled = on; }

    // Print string from memory.
    void print_string(std::ostream &out, unsigned addr);

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

    // Read number from stream.
    static long double input_real(std::istream &stream);

    // Read character from stdin.
    static int input_char(std::istream &stream);

    // Read console switches.
    unsigned read_console_switches(std::istream &stream, unsigned bitmask);

    // Ask operator for console input.
    void ask_console_input() { switches_are_valid = false; }

    // Print number in floating-point representation.
    static void print_floating_point(std::ostream &stream, int n, int m, long double x);

    // Print number in fixed-point representation, with or without a sign.
    static void print_fixed_point(std::ostream &stream, int n, int m, long double x,
                                  bool need_sign);

    // Print number in fixed-point or floating-point representation.
    static void print_int_or_real(std::ostream &stream, long double x);

    //
    // Trace methods.
    //
    static void trace_exception(const char *message)
    {
        if (trace_enabled())
            print_exception(message);
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

    static void trace_stack(unsigned offset, const std::string &value, const std::string &op)
    {
        if (debug_flag)
            print_stack_op(offset, value, op);
    }

    static void trace_display(unsigned level, unsigned value)
    {
        if (debug_flag)
            print_display(level, value);
    }

    void trace_level(unsigned level, unsigned frame)
    {
        if (debug_flag)
            print_level(level, frame);
    }

    void trace_level()
    {
        if (debug_flag)
            print_level();
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
    static void print_memory_access(unsigned addr, Word val, const char *opname);
    static void print_stack_op(unsigned offset, const std::string &value, const std::string &op);
    static void print_display(unsigned level, unsigned value);
    void print_level(unsigned level, unsigned frame);
    void print_level();
};

//
// Decode word as string.
//
std::string word_string(Word w);

#endif // X1_MACHINE_H
