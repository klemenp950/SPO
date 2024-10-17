#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define err(...) { fprintf(stderr, __VA_ARGS__); exit(1); }

void printLastNLines(int n, char *filename);

int main(int argc, char *argv[]){
    if(argc != 2 && argc != 4){
        err("Usage: %s [-n] <Filename>\n", argv[0]);
    }

    if(argc == 2){
        char *filename = argv[1];
        printLastNLines(5, filename);
    }

    if(argc == 4){
        if(strcmp(argv[1], "-n") != 0){
            err("Usage: %s [-n] <Filename>\n", argv[0]);
        }
        int n = atoi(argv[2]);
        char *filename = argv[3];
        printLastNLines(n, filename);
    }

    return 0;
}

void printLastNLines(int n, char *filename){
    int fd;

    if((fd = open(filename, O_RDONLY)) < 0){
        err("open failed\n");
    }

    off_t offset = lseek(fd, 0, SEEK_END) - 1;

    while(n > 0){
        char temp;
        lseek(fd, offset, SEEK_SET); 
        if (read(fd, &temp, 1) != 1) {
            err("read failed\n");
        }
        if (temp == '\n'){
            n--;
        }
        offset--;
    }

    char buf[32];
    int len;
    while((len = read(fd, buf, 32)) > 0){
        for(int i = 0; i < len; i++){
            fprintf(stdout, "%c", buf[i]);
        }
    }

    close(fd);
}
    