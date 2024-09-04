#include <iostream>
#include <limits>

#include "machine.h"

#ifdef _WIN32
#include <io.h>
bool Machine::is_interactive = _isatty(0);
#else
#include <unistd.h>
bool Machine::is_interactive = isatty(0);
#endif

//
// Read real or integer number from input stream.
// Return the number.
// On end of file, throw exception.
// When input is invalid:
//  - throw exception when non-interactive
//  - retry when in interactive mode
//
long double Machine::input_real(std::istream &input_stream)
{
    // Loop until user enters a valid input
    for (;;) {
        if (is_interactive) {
            std::cout << "Enter number: " << std::flush;
        }
        long double x{};
        input_stream >> x;

        if (input_stream) {
            // Number is valid.
            return x;
        }

        // Cannot parse the input.
        if (input_stream.eof()) {
            throw std::runtime_error("No input");
        }
        if (!is_interactive) {
            throw std::runtime_error("Bad input");
        }
        std::cout << "Bad input, try again.\n";
        input_stream.clear();

        // Remove bad input and try again.
        input_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

//
// Read console switches.
// Called by XEEN procedure.
//
unsigned Machine::read_console_switches(std::istream &input_stream, unsigned bitmask)
{
    if (!switches_are_valid) {
        // Loop until user enters a valid input
        for (;;) {
            if (is_interactive) {
                std::cout << "\n";
                std::cout << "Enter console switches (up to 27 binary digits)\n";
                std::cout << ": " << std::flush;
            }
            try {
                std::string line;
                if (std::getline(input_stream, line)) {
                    console_switches = std::stoul(line, nullptr, 2);
                    break;
                }
            } catch (...) {
                // No valid number.
            }

            // Cannot parse the input.
            if (input_stream.eof()) {
                throw std::runtime_error("No input");
            }
            if (!is_interactive) {
                throw std::runtime_error("Bad input");
            }
            std::cout << "Bad input, try again.\n";
            input_stream.clear();

            // Remove bad input and try again.
            input_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return console_switches & bitmask;
}
