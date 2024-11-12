#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define err(...) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define BUFFSIZE 512

int main(int argc, char *argv[]){
    if(argc == 1){
        err("Usecase: %s [-a] <filename>\n", argv[0])
        return 1;
    }

    if(argc == 2){
        int fdDat;
        int n;

        if(strcmp(argv[1], "-a") == 0){
            err("Usecase: %s [-a] <filename>\n", argv[0]);
            return 1;
        }

        if((fdDat = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0){
            err("Error while opening file.\n");
            return 1;
        }
        
        char buf[BUFFSIZE];
        while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0){
            if(write(fdDat, buf, n) != n){
                err("Error while writing to file\n");
                return 1;
            }
            fprintf(stdout, "%s", buf);
        }

        
        if(n < 0){
            err("Error while reading from stdin\n");
            return 1;
        }
    }

    if(argc == 3){
        if(strcmp(argv[1], "-a") != 0){
            err("Wrong argument\n");
            return 1;
        }

        int fdDat;
        int n;

        if((fdDat = open(argv[2], O_WRONLY | O_APPEND | O_CREAT)) < 0){
            err("Error while openenig file.\n");
            return 1;
        }

        char buf[BUFFSIZE];
        while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0){
            if(write(fdDat, buf, n) != n){
                err("Error while writing to file\n");
                return 1;
            }
            fprintf(stdout, "%s", buf);
        }
    }
}