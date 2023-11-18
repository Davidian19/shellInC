#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>

char last_input[2048] = "";
int countPid = 0;

void copyLastInput(char *input, const char *last_input) {
    strncpy(input, last_input, 2048 - 1);
    input[2048 - 1] = '\0';
}

void exec(char *command, int runInBackground){
    char *cmd = strdup(command);
    char *commands[2]; 
    int command_count = 0;
    char *in_file = NULL;
    char *out_file = NULL;
    char *args[1024];

    char *token = strtok(cmd, "|");
    while (token != NULL) {
        commands[command_count] = strdup(token);
        command_count++;
        if (command_count >= 2) {
            break;
        }
        token = strtok(NULL, "|");
    }

    if (command_count == 2) {
        int pipe_fds[2];
        if (pipe(pipe_fds) == -1) {
            perror("Erro ao criar o pipe");
            exit(1);
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            close(pipe_fds[0]);
            dup2(pipe_fds[1], STDOUT_FILENO);
            close(pipe_fds[1]);

            token = strtok(commands[0], " ");
            int i = 0;
            while (token != NULL) {
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            execvp(args[0], args);
            perror("Erro ao executar o primeiro comando");
            exit(1);
        } else if (pid1 < 0) {
            perror("Erro ao criar o processo filho");
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            close(pipe_fds[1]);
            dup2(pipe_fds[0], STDIN_FILENO);
            close(pipe_fds[0]);

            token = strtok(commands[1], " ");
            int i = 0;
            while (token != NULL) {
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            execvp(args[0], args);
            perror("Erro ao executar o segundo comando");
            exit(1);
        } else if (pid2 < 0) {
            perror("Erro ao criar o processo filho");
        }

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        int status;
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
    } else if (command_count == 1) {
        token = strtok(commands[0], " ");
        int i = 0;
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        for (int j = 0; j < i; j++) {
            if (strcmp(args[j], ">") == 0) {
                if (j + 1 < i) {
                    out_file = args[j + 1];
                    args[j] = NULL;
                    break;
                }
            } else if (strcmp(args[j], ">>") == 0) {
                if (j + 1 < i) {
                    out_file = args[j + 1];
                    args[j] = NULL;
                    break;
                }
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (out_file != NULL) {
                int out_fd;
                if (strcmp(args[0], ">>") == 0) {
                    out_fd = open(out_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                } else {
                    out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                }
                if (out_fd == -1) {
                    perror("Erro ao abrir o arquivo de saída");
                    exit(1);
                }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }

            execvp(args[0], args);
            perror("Erro ao executar o comando");
            exit(1);
        } else if (pid < 0) {
            perror("Erro ao criar o processo filho");
        } else {
            if (!runInBackground) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                countPid++;
                printf("[%d] %d\n", countPid, getpid());
            }
        }
        free(cmd);
    }
}


void executeCommand(char *commands) {
    char *cmd = strdup(commands);
    int runInBackground = 0;

    if (cmd[strlen(cmd) - 1] == '&') {
        cmd[strlen(cmd) - 1] = '\0';
        runInBackground = 1;
    }

    exec(strdup(cmd), runInBackground);

    free(cmd);
}

int main(int argc, char *argv[]) {

    char input[2049];

    if (argc == 2) {
        char *saveptr = NULL;
        FILE *batchFile = fopen(argv[1], "r");
        if (batchFile == NULL) {
            perror("Erro ao abrir o arquivo de lote (batch)");
            return 1;
        }

        char input[1024];
        char last_input[1024];
        while (fgets(input, sizeof(input), batchFile) != NULL) {
            input[strcspn(input, "\n")] = 0;

            char *command = strdup(input);
            copyLastInput(last_input, input);
            printf("dipp> %s\n", input);

            char *saveptr_batch = NULL;
            char *cmd = strtok_r(command, ";", &saveptr_batch);
            while (cmd != NULL) {
                if(strcmp(cmd, "exit") == 0){
                    break;
                }
                executeCommand(cmd);
                cmd = strtok_r(NULL, ";", &saveptr_batch);
            }

            free(command);
        }

        fclose(batchFile);
    } else {
        int parallel_mode = 0;
        char *saveptr = NULL;

        while (!feof(stdin)) {
            printf("dipp %s> ", parallel_mode ? "par" : "seq");
            fgets(input, sizeof(input), stdin);

            input[strlen(input) - 1] = '\0';

            if (strcmp(input, "style parallel") == 0) {
                parallel_mode = 1;
                continue;
            } else if (strcmp(input, "style sequential") == 0) {
                parallel_mode = 0;
                continue;
            } else if (strcmp(input, "!!") == 0) {
                if (strlen(last_input) > 0) {
                    char last_input_copy[2048];
                    copyLastInput(last_input_copy, last_input);
                    executeCommand(last_input_copy);
                }
            } else if (strcmp(input, "exit") == 0) {
                break;
            } else {
                copyLastInput(last_input, input);

                if (parallel_mode) {
                    int thread_count = 0;

                    char *command = strdup(input);
                    char *saveptr;
                    char *cmd = strtok_r(command, ";", &saveptr);
                    while (cmd != NULL) {
                        executeCommand(cmd);
                        cmd = strtok_r(NULL, ";", &saveptr);
                    }

                    free(command);
                } else {
                    char *saveptr;
                    char *commands[64];
                    int command_count = 0;

                    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
                        commands[i] = NULL;
                    }

                    char *command = strdup(input);
                    char *cmd = strtok_r(command, ";", &saveptr);

                    while (cmd != NULL) {
                        commands[command_count] = strdup(cmd);
                        command_count++;
                        if (command_count >= sizeof(commands) / sizeof(commands[0])) {
                            break;
                        }

                        cmd = strtok_r(NULL, ";", &saveptr);
                    }

                    for (int i = 0; i < command_count; i++) {
                        char *cmd = commands[i];
                        executeCommand(cmd);
                        free(cmd);
                    }
                    free(command);
                }
            }
        }
    }

    return 0;
}
