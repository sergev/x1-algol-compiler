#include "fixture.h"

#include <fstream>
#include <cstring>

//
// Compile Algol sources and store binaries in memory.
//
void x1_machine::compile(const std::string &source_code)
{
    const auto algol_filename = test_name + ".a60";
    const auto obj_filename   = test_name + ".x1";

    create_file(algol_filename, source_code);

    machine->compile(std::vector<std::string>(1, algol_filename), obj_filename);
    machine->load_object_program(obj_filename);
}

//
// Compile and run Algol code.
// Return captured output.
//
std::string x1_machine::compile_and_run(const std::string &source_code)
{
    compile(source_code);

    // Redirect stdout.
    std::streambuf *save_cout = std::cout.rdbuf();
    std::ostringstream output;
    std::cout.rdbuf(output.rdbuf());

    // Run the program.
    EXPECT_NO_THROW(machine->run(machine->get_entry(0)));

    // Return output.
    std::cout.rdbuf(save_cout);
    return output.str();
}

//
// Read file contents and return it as a string.
//
std::string file_contents(const std::string &filename)
{
    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << filename << ": " << std::strerror(errno) << std::endl;
        return "";
    }
    std::stringstream contents;
    contents << input.rdbuf();
    return contents.str();
}

//
// Read file contents as vector of strings.
//
std::vector<std::string> split_stream(std::istream &input)
{
    std::vector<std::string> output;
    std::string line;
    while (std::getline(input, line)) {
        output.push_back(line);
    }
    return output;
}

//
// Read file contents as vector of strings.
//
std::vector<std::string> file_contents_split(const std::string &filename)
{
    std::ifstream input(filename);
    return split_stream(input);
}

//
// Read file contents as vector of strings.
//
std::vector<std::string> multiline_split(const std::string &multiline)
{
    std::stringstream input(multiline);
    return split_stream(input);
}

//
// Read FILE* stream contents until EOF and return it as a string.
//
std::string stream_contents(FILE *input)
{
    std::stringstream contents;
    char line[256];
    while (fgets(line, sizeof(line), input)) {
        contents << line;
    }
    return contents.str();
}

//
// Create file with given contents.
//
void create_file(const std::string &filename, const std::string &contents)
{
    std::ofstream output(filename);
    output << contents;
}

//
// Create file with given contents.
//
void create_file(const std::string &dest_filename, const std::string &prolog,
                 const std::string &src_filename, const std::string &epilog)
{
    std::ofstream output(dest_filename);
    EXPECT_TRUE(output.is_open()) << dest_filename;

    std::ifstream input(src_filename);
    EXPECT_TRUE(input.is_open()) << src_filename;

    output << prolog;
    output << input.rdbuf();
    output << epilog;
}

//
// Check whether string starts with given prefix.
//
bool starts_with(const std::string &str, const char *prefix)
{
    auto prefix_size = strlen(prefix);
    return str.size() >= prefix_size && memcmp(str.c_str(), prefix, prefix_size) == 0;
}

//
// Compare output of simulation.
// Ignore footer.
//
void check_output(const std::string &output_str, const std::string &expect_str)
{
    std::stringstream output(output_str);
    std::stringstream expect(expect_str);

    // Compare line by line.
    for (unsigned lineno = 1; expect.good(); lineno++) {
        ASSERT_TRUE(output.good()) << "Output is too short";

        std::string output_line;
        getline(output, output_line);
        if (output_line == "------------------------------------------------------------")
            break;

        // Remove trailing spaces.
        output_line.resize(1 + output_line.find_last_not_of(' '));

        std::string expect_line;
        getline(expect, expect_line);
        EXPECT_EQ(output_line, expect_line)
            << "line #" << lineno;
    }
}
