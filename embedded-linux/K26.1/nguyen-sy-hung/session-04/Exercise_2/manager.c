#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_SIZE             100U
#define EXEC_PATH              "./searcher"
#define STUDENT_FILE_PATH      "students.txt"
#define QUIT_COMMAND           "quit"
#define EXEC_ERROR_CODE        2

extern char **environ;


static void remove_newline(char *str)
{
    size_t len = strlen(str);

    if ((len > 0U) && (str[len - 1U] == '\n'))
    {
        str[len - 1U] = '\0';
    }
}


static void print_exit_status(int status, pid_t pid)
{
    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);

        switch (exit_code)
        {
            case EXIT_SUCCESS:
                printf("[MANAGER] Child PID %d: Student found\n",
                        pid);
                break;

            case 1:
                printf("[MANAGER] Child PID %d: Student not found\n",
                        pid);
                break;

            case EXEC_ERROR_CODE:
                printf("[MANAGER] Child PID %d: Error occurred\n",
                        pid);
                break;

            default:
                printf("[MANAGER] Child PID %d: Unknown exit code %d\n",
                        pid,
                        exit_code);
                break;
        }
    }
    else
    {
        printf("[MANAGER] Child PID %d terminated abnormally\n",
                pid);
    }
}


int main(void)
{
    char student_id[INPUT_SIZE];

    printf("====================================\n");
    printf(" STUDENT LOOKUP SYSTEM - MANAGER\n");
    printf("====================================\n");

    printf("[MANAGER] PID: %d\n", getpid());


    while (1)
    {
        printf("\nEnter student ID ('quit' to exit): ");

        if (fgets(student_id,
                  sizeof(student_id),
                  stdin) == NULL)
        {
            perror("fgets failed");
            return EXIT_FAILURE;
        }


        remove_newline(student_id);


        if (strcmp(student_id, QUIT_COMMAND) == 0)
        {
            printf("[MANAGER] Goodbye\n");
            break;
        }


        pid_t pid = fork();


        if (pid < 0)
        {
            perror("fork failed");
            continue;
        }


        if (pid == 0)
        {
            char *args[] =
            {
                EXEC_PATH,
                student_id,
                STUDENT_FILE_PATH,
                NULL
            };


            execve(EXEC_PATH, args, environ);


            /*
             * This line is reached only if execve() fails.
             * On success, execve() replaces the current
             * process image with searcher program.
             */
            perror("execve failed");

            exit(EXEC_ERROR_CODE);
        }


        printf("[MANAGER] Created child PID: %d\n",
                pid);


        int status = 0;


        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            continue;
        }


        print_exit_status(status, pid);
    }


    return EXIT_SUCCESS;
}