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
int commandCounter;

void handle_sigint(int sig);
void handle_sigusr1(int sig);
void parse(char *niz_ukaz, char *args[]);
void parse2(char *niz_ukaz, char *args[]);
void setPwd();

int main(){
    commandCounter = 0;
    setPwd();
    signal(SIGINT, handle_sigint);
    signal(SIGUSR1, handle_sigusr1);
    while (1)
    {   
        printf("Shell:%s$ ", pwd);
        if(fgets(input, buffer_size, stdin) == NULL){
            printf("Exiting shell\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        int status = fork();
        commandCounter++;
        if(status == 0){
            char *args[argument_size];
            if(commandCounter % 2 == 0){
                parse(input, args);
            } else {
                parse2(input, args);
            }

            if(execvp(args[0], args) < 0){
                printf("Command not found\n");
            }
        } else {
            wait(NULL);
        }
    }
    
}

void parse(char *niz_ukaz, char *args[]){
    int length = strlen(niz_ukaz);
    int counterWordlength = 0;
    int pointerCounter = 0;
    char currentCharacther;
    for (size_t i = 0; i < length; i++)
    {
        currentCharacther = niz_ukaz[i];
        if(currentCharacther == ' '){
            niz_ukaz[i] = '\0';
            if(i-counterWordlength >= 0 && niz_ukaz[i-counterWordlength] != ' '){
                args[pointerCounter] = &niz_ukaz[i - counterWordlength];
                pointerCounter++;
                counterWordlength = 0;
            }
        } else {
            counterWordlength++;
        }
    }

    if (counterWordlength > 0) {
        args[pointerCounter] = &niz_ukaz[length - counterWordlength];
        pointerCounter++;
    }

    args[pointerCounter] = NULL;
}

void parse2(char *niz_ukaz, char *args[]) {
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