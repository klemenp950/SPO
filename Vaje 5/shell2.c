#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define buffer_size 1024
#define argument_size 64

char input[buffer_size];
char pwd[buffer_size];

void handle_sigint(int sig);
void handle_sigusr1(int sig);
void parse2(char *niz_ukaz, char *args[]);
void setPwd();
void split(char *ukaz, char *left, char *right);

int main(){
    setPwd();
    signal(SIGINT, handle_sigint);
    signal(SIGUSR1, handle_sigusr1);
    pid_t pid, cpid;
    char* args[argument_size], *args2[argument_size];
    char left[buffer_size], right[buffer_size];
    int fd[2];
    
    while(1){
        printf("ShellKlemen$ ");
        if(fgets(input, buffer_size, stdin) == NULL){
            printf("Exiting shell\n");
            fflush(stdout);
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }
        if (pid == 0) { // Child process
            split(input, left, right);

            if (strlen(right) == 0) {  // Simple command
                parse2(left, args);
                if (execvp(args[0], args) < 0) {
                    printf("Command not found\n");
                }
                exit(1);
            } else { // Pipe
                if (pipe(fd) < 0) {
                    perror("pipe");
                    exit(1);
                }
                if ((cpid = fork()) < 0) {
                    perror("fork");
                    exit(1);
                }
                if (cpid == 0) { // P3
                    close(fd[1]);
                    dup2(fd[0], 0);
                    close(fd[0]);

                    parse2(right, args2);
                    if (execvp(args2[0], args2) != 0) {
                        perror("Napaka desni del");
                    }
                    exit(1);
                } else { // P2
                    close(fd[0]);
                    dup2(fd[1], 1);
                    close(fd[1]);

                    parse2(left, args);
                    if (execvp(args[0], args) < 0) {
                        printf("Command not found\n");
                    }
                    waitpid(cpid, NULL, 0);
                    exit(1);
                }
            }
        } else { // Parent process
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}

void parse2(char *niz_ukaz, char *args[]){
    int i = 0;
    char *token;

    token = strtok(niz_ukaz, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    args[i] = NULL;
}

void handle_sigint(int sig) {
    printf("Caught SIGINT (Ctrl+C). Use Ctrl+D to quit the shell.\nShell:%s$ ", pwd);
    fflush(stdout);
}

void handle_sigusr1(int sig) {
    printf("Caught SIGUSR1.\nShell:%s$ ", pwd);
    fflush(stdout);
}

void setPwd(){
    getcwd(pwd, sizeof(pwd));
}

void split(char *ukaz, char *left, char *right) {
    char *delim = strstr(ukaz, " => ");
    if (delim != NULL) {
        *delim = '\0';
        strcpy(left, ukaz);
        strcpy(right, delim + 4);
    } else {
        strcpy(left, ukaz);
        strcpy(right, "\0");
    }
}