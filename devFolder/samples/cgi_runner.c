#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv) {
    // Fork a child process
    
    if(strcmp(argv[1], "../cgi-bin/create_file.py") == 0)
    {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            
            // Execute the Python script using exec
            execlp("python", "python", argv[1], NULL);
            
            // If exec returns, an error occurred
            perror("exec");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            
            // Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);
            
            // Check the exit status of the child process
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                printf("Child process exited with status: %d\n", exit_status);
            } else {
                printf("Child process terminated abnormally.\n");
            }
        } else {
            // Fork failed
            perror("fork");
            exit(1);
        }
    }

    return 0;
}