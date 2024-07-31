#ifndef X1_PROCESSOR_H
#define X1_PROCESSOR_H

#include <cstdint>
#include <string>

#include "x1_arch.h"

class Machine;

//
// Internal state of the processor.
//
struct CoreState {
    unsigned B;  // stack address + sign bit
    Word A;      // accumulator
    Word S;      // extension of accumulator
    bool C;      // condition
};

//
// Electrologica X1 processor.
//
class Processor {
private:
    // Reference to the machine.
    Machine &machine;

    // Current state.
    struct CoreState core{};

    // Previous state, for tracing.
    struct CoreState prev{};

    unsigned OT{}; // order counter
    unsigned OR{}; // order register

public:
    // Exception for unexpected situations.
    class Exception : public std::exception {
    private:
        std::string message;

    public:
        explicit Exception(const std::string &m) : message(m) {}
        const char *what() const noexcept override { return message.c_str(); }
    };

    // Constructor.
    explicit Processor(Machine &mach) : machine(mach) { reset(); }

    // Reset to initial state.
    void reset();

    // Simulate one instruction.
    // Return true when the processor is stopped.
    bool step();

    // Set register value.
    void set_ot(unsigned val) { OT = val; }
    void set_b(unsigned val) { core.B = val; }
    void set_c(bool val) { core.C = val; }
    void set_a(Word val) { core.A = val; }
    void set_s(Word val) { core.S = val; }

    // Get register value.
    unsigned get_ot() const { return OT; }
    unsigned get_b() const { return core.B; }
    bool get_c() const { return core.C; }
    Word get_a() const { return core.A; }
    Word get_s() const { return core.S; }

    // Print trace info.
    void print_instruction();
    void print_registers();
};

#endif // X1_PROCESSOR_H
