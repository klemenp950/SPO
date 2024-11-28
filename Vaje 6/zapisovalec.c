#include<sys/msg.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>

struct mymesg {
	long mtype;
	char mtext[2048];
};

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    int fd;
    int msqid;
    key_t key_msgq;
    char message[2048];
    struct mymesg msgbuf;

    if((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0){
        perror("open");
    }

    if((key_msgq = ftok("/usr", 'c')) < 0){
        fprintf(stderr, "Error: ftok\n");
        exit(1);
    }

    if((msqid = msgget(key_msgq, 0644)) < 0){
        perror("msgget");
    }

    do
    {
        if((msgrcv(msqid, &msgbuf, sizeof(msgbuf.mtext), 0, 0)) < 0){
            perror("msgrcv");
        }
        dprintf(fd, "%s", msgbuf.mtext);
    } while (msgbuf.mtext != "\0");
    
    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}