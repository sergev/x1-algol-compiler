#ifndef X1_TESTS_UTIL_H
#define X1_TESTS_UTIL_H

#include <gtest/gtest.h>

#include <random>
#include <string>

#include "x1_arch.h"
#include "machine.h"

//
// Fixture with preallocated machine.
//
// For details, see: https://github.com/google/googletest/blob/main/docs/primer.md
//
class x1_machine : public ::testing::Test {
protected:
    std::unique_ptr<Machine> machine;

    void SetUp() override
    {
        // Allocate fresh new machine.
        machine = std::make_unique<Machine>();
    }
};

//
// Get current test name, as specified in TEST() macro.
//
std::string get_test_name();

//
// Read file contents and return it as a string.
//
std::string file_contents(const std::string &filename);

//
// Read file contents as vector of strings.
//
std::vector<std::string> file_contents_split(const std::string &filename);

//
// Split multi-line text as vector of strings.
//
std::vector<std::string> multiline_split(const std::string &multiline);

//
// Read FILE* stream contents and return it as a string.
//
std::string stream_contents(FILE *input);

//
// Create file with given contents.
//
void create_file(const std::string &filename, const std::string &contents);
void create_file(const std::string &dest_filename, const std::string &prolog,
                 const std::string &src_filename, const std::string &epilog);

//
// Check whether string starts with given prefix.
//
bool starts_with(const std::string &str, const char *prefix);

//
// Compare output of simulation.
// Ignore footer.
//
void check_output(const std::string &output_str, const std::string &expect_str);

#endif // X1_TESTS_UTIL_H