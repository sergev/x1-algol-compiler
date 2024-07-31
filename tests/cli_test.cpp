#include <cstdio>
#include <fstream>

#include "fixture.h"

TEST(cli, usage)
{
    // Run simulator via shell.
    FILE *pipe = popen("../simulator/x1sim --help", "r");
    ASSERT_TRUE(pipe != nullptr);

    // Capture the output.
    std::string result = stream_contents(pipe);
    std::cout << result;

    // Check exit code.
    int exit_status = pclose(pipe);
    int exit_code   = WEXITSTATUS(exit_status);
    ASSERT_NE(exit_status, -1);
    ASSERT_EQ(exit_code, 0);

    // Check output.
    EXPECT_NE(result.find("Version"), std::string::npos);
    EXPECT_NE(result.find("Usage:"), std::string::npos);
    EXPECT_NE(result.find("Options:"), std::string::npos);
}

TEST(cli, version)
{
    // Run simulator via shell.
    FILE *pipe = popen("../simulator/x1sim --version", "r");
    ASSERT_TRUE(pipe != nullptr);

    // Capture the output.
    std::string result = stream_contents(pipe);
    std::cout << result;

    // Check exit code.
    int exit_status = pclose(pipe);
    int exit_code   = WEXITSTATUS(exit_status);
    ASSERT_NE(exit_status, -1);
    ASSERT_EQ(exit_code, 0);

    // Check output.
    EXPECT_NE(result.find("Version"), std::string::npos);
    EXPECT_NE(result.find("."), std::string::npos);
    EXPECT_NE(result.find("-"), std::string::npos);
}
