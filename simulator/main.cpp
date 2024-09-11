#include <getopt.h>

#include <cstring>
#include <iostream>

#include "machine.h"

//
// CLI options.
//
static const struct option long_options[] = {
    // clang-format off
    { "help",           no_argument,        nullptr,    'h' },
    { "version",        no_argument,        nullptr,    'V' },
    { "trace",          required_argument,  nullptr,    'T' },
    { "debug",          required_argument,  nullptr,    'd' },
    { nullptr },
    // clang-format on
};

//
// Print usage message.
//
static void print_usage(std::ostream &out, const char *prog_name)
{
    out << "X1 Algol Simulator, Version " << VERSION_STRING << "\n";
    out << "Usage:\n";
    out << "    " << prog_name << " [options...] file.[a60|x1] [library ...]\n";
    out << "Input files:\n";
    out << "    file.a60                Algol-60 program source\n";
    out << "    file.x1                 Binary program for Electrologica X1\n";
    out << "    library                 If a source file is given, the library file(s)\n";
    out << "                            are passed to the compiler\n";
    out << "Options:\n";
    out << "    -h, --help              Display available options\n";
    out << "    -V, --version           Print the version number and exit\n";
    out << "    --trace=FILE            Redirect trace to the file\n";
    out << "    -d, --debug             Enable tracing\n";
}

//
// Main routine of the simulator,
// when invoked from a command line.
//
int main(int argc, char *argv[])
{
    // Get the program name.
    const char *prog_name = strrchr(argv[0], '/');
    if (prog_name == nullptr) {
        prog_name = argv[0];
    } else {
        prog_name++;
    }

    // Instantiate the machine.
    Machine machine;

    // Use C version of the Algol compiler by default.
    machine.set_compiler("x1algc");

    // Parse command line options.
    for (;;) {
        switch (getopt_long(argc, argv, "-hVT:d", long_options, nullptr)) {
        case EOF:
            break;

        case 0:
            continue;

        case 1:
            // Regular argument.
            machine.add_input_file(optarg);
            continue;

        case 'h':
            // Show usage message and exit.
            print_usage(std::cout, prog_name);
            exit(EXIT_SUCCESS);

        case 'V':
            // Show version and exit.
            std::cout << "Version " << VERSION_STRING << "\n";
            exit(EXIT_SUCCESS);

        case 'T':
            // Redirect tracing to given file.
            Machine::redirect_trace(optarg);
            Machine::get_trace_stream() << "X1 Algol Simulator Version: " << VERSION_STRING << "\n";
            continue;

        case 'd':
            // Set trace options.
            machine.enable_trace();
            continue;

        default:
            print_usage(std::cerr, prog_name);
            exit(EXIT_FAILURE);
        }
        break;
    }

    // Must specify an Algol file or an object file.
    if (machine.input_file_count() < 1) {
        print_usage(std::cerr, prog_name);
        exit(EXIT_FAILURE);
    }

    try {
        if (machine.input_file_count() == 1) {
            machine.find_default_library(argv[0]);
        }
        machine.compile_and_run();

    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return machine.get_exit_status();
}
