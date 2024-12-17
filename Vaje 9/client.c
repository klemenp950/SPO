#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define MAX_LEN 1024

int N;
pthread_t *thread;

void* send_to_server(void* arg);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Socket(int domain, int type, int protocol);

int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Usage: %s N", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);

    thread = malloc(sizeof(pthread_t) * N);

    for (int i = 0; i < N; i++) {
        int *thread_id = malloc(sizeof(int));
        *thread_id = i + 1;
        pthread_create(&thread[i], NULL, send_to_server, thread_id);
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(thread[i], NULL);
        printf("Joined thread %d\n", i);
    }

    return 0;
}

void* send_to_server(void* arg){
    int* i = (int *) arg;

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); // 1234 is the port number
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); // 127.0.0.1 is the IP address

    Connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

    char buffer_write[MAX_LEN];
    snprintf(buffer_write, sizeof(buffer_write), "%d", *i);
    write(sockfd, buffer_write, strlen(buffer_write));

    char buffer_read[MAX_LEN];
    int temp;

    while ((temp = read(sockfd, buffer_read, MAX_LEN - 1)) > 0){
        buffer_read[temp] = '\0';
        printf("%s\n", buffer_read);
    }

    close(sockfd);
    exit(0);
    return NULL;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = connect(sockfd, addr, addrlen);
    if(ret < 0) {
        perror("connect");
        exit(1);
    }
    return ret;
}

int Socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if(sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}