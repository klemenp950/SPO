#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LEN 1024

int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void handle_client(int connfd, struct sockaddr_in addr);
pid_t Fork();
void sig_chld(int signo);

int main(int argc , char* argv[]){
    signal(SIGCHLD, sig_chld);
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); // 1234 is the port number
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); // 127.0.0.1 is the IP address

    Bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
    printf("Server is running on %s:%d\n", ip_str, ntohs(addr.sin_port));

    Listen(sockfd, 5);
    
    while (1)
    {
        int connfd = Accept(sockfd, NULL, NULL);
        pid_t pid = Fork();

        if(pid == 0){ // Child process
            handle_client(connfd, addr);
            close(connfd);
            exit(0);
        } 
        close(connfd);
    }
}

int Socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if(sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = bind(sockfd, addr, addrlen);
    if(ret < 0) {
        perror("bind");
        exit(1);
    }
    return ret;
}

int Listen(int sockfd, int backlog) {
    int ret = listen(sockfd, backlog);
    if(ret < 0) {
        perror("listen");
        exit(1);
    }
    return ret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int ret = accept(sockfd, addr, addrlen);
    if(ret < 0) {
        perror("accept");
        exit(1);
    }
    return ret;
}

pid_t Fork() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    return pid;
}

void handle_client(int connfd, struct sockaddr_in addr){
    char buffer_read[MAX_LEN];
    int temp;
    while ((temp = read(connfd, buffer_read, sizeof(buffer_read) - 1)) > 0)
    {
        buffer_read[temp] = '\0';
        int i = atoi(buffer_read);

        char buffer_write[MAX_LEN];
        if(getpid() % i == 0){
            snprintf(buffer_write, sizeof(buffer_write), "Nit %d: Čestitam, dobili ste nagrado!\n", i);
            write(connfd, buffer_write, strlen(buffer_write));
            close(connfd);

        } else {
            snprintf(buffer_write, sizeof(buffer_write), "Nit %d: Važno je sodelovati, ne zmagati.\n", i);
            write(connfd, buffer_write, strlen(buffer_write));
            close(connfd);
        }

        char ip_str[MAX_LEN];
        inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);

        printf("Otrok %d streznika sem stregel niti %i odjemalca (%s:%d)\n", getpid(), i, ip_str, ntohs(addr.sin_port));
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    
    return;
}