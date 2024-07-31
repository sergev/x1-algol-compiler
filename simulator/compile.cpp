#include "machine.h"

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum {
    STATUS_CANNOT_READ_INPUT = 100,
    STATUS_CANNOT_WRITE_OUTPUT,
    STATUS_CANNOT_EXECUTE_COMPILER,
};

void Machine::run_program(const std::string &prog_path, const std::string &input_filename, const std::string &output_filename)
{
    pid_t pid = fork();
    if (pid < 0) {
        //TODO: perror("fork");
        return;
    }

    if (pid == 0) {
        //
        // Child process.
        //

        // Open input file.
        int in_fd = open(input_filename.c_str(), O_RDONLY);
        if (in_fd == -1) {
            exit(STATUS_CANNOT_READ_INPUT);
        }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);

        // Create output file.
        int out_fd = open(output_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd == -1) {
            exit(STATUS_CANNOT_WRITE_OUTPUT);
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);

        // Start compiler.
        execlp(prog_path.c_str(), prog_path.c_str(), nullptr);
        exit(STATUS_CANNOT_EXECUTE_COMPILER);
    }

    //
    // Parent process.
    //
    int exit_status;
    waitpid(pid, &exit_status, 0);
    if (exit_status < 0) {
        //TODO: perror("waitpid");
        return;
    }

    int exit_code = WEXITSTATUS(exit_status);
    switch (exit_code) {
    case STATUS_CANNOT_READ_INPUT:
        //TODO
        break;
    case STATUS_CANNOT_WRITE_OUTPUT:
        //TODO
        break;
    case STATUS_CANNOT_EXECUTE_COMPILER:
        //TODO
        break;
    }
}
