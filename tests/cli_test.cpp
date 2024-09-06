#include <cstdio>
#include <fstream>

#include "fixture.h"

static void run_command(std::string &result, const std::string &cmd)
{
    // Run simulator via shell.
    FILE *pipe = popen(cmd.c_str(), "r");
    ASSERT_TRUE(pipe != nullptr);

    // Capture output.
    result = stream_contents(pipe);
    std::cout << result;

    // Check exit code.
    int exit_status = pclose(pipe);
    int exit_code   = WEXITSTATUS(exit_status);
    ASSERT_NE(exit_status, -1);
    ASSERT_EQ(exit_code, 0);
}

TEST(cli, usage)
{
    std::string result;
    run_command(result, "../simulator/x1sim --help");

    // Check output.
    EXPECT_NE(result.find("Version"), std::string::npos);
    EXPECT_NE(result.find("Usage:"), std::string::npos);
    EXPECT_NE(result.find("Options:"), std::string::npos);
}

TEST(cli, version)
{
    std::string result;
    run_command(result, "../simulator/x1sim --version");

    // Check output.
    EXPECT_NE(result.find("Version"), std::string::npos);
    EXPECT_NE(result.find("."), std::string::npos);
    EXPECT_NE(result.find("-"), std::string::npos);
}

TEST(cli, print123_a60)
{
    std::string result;
    run_command(result, "../simulator/x1sim " TEST_DIR "/print123.a60");

    EXPECT_EQ(result, "123\n");
}

TEST(cli, print123_x1)
{
    std::string result;
    run_command(result, "../simulator/x1sim " TEST_DIR "/print123.x1");

    EXPECT_EQ(result, "123\n");
}

TEST(cli, lib_sum)
{
    std::string result;
    run_command(result, "../simulator/x1sim " TEST_DIR "/../examples/sum.a60 " TEST_DIR "/../library/sum.lib");

    // This example shoult print 55*20 = 1100.
    EXPECT_EQ(result, "1100\n");
}
