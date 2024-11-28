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

    char prev[2048] = "";

    while (1)
    {
        if((msgrcv(msqid, &msgbuf.mtext, sizeof(msgbuf.mtext), 0, 0)) < 0){
            perror("msgrcv");
        }

        if (msgbuf.mtext[0] == '\0')
        {
            break;
        }
        

        if (strcmp(prev, msgbuf.mtext) != 0)
        {
            printf("%s\n", msgbuf.mtext);
            write(fd, msgbuf.mtext, strlen(msgbuf.mtext));
            strcpy(prev, msgbuf.mtext);
        }
    }
    
    
    
    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}