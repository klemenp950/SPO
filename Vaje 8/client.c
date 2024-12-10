#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Socket(int domain, int type, int protocol);

int main(){
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); // 1234 is the port number
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); // 127.0.0.1 is the IP address

    Connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

    char buffer[1024];
    int temp;
    while((temp = read(sockfd, buffer, sizeof(buffer))) > 0) {
        buffer[temp] = '\0';
        printf("%s", buffer);
        if (strcmp(buffer, "Še ena karta? (H/S)\n") == 0)
        {
            char odgovor[2];
            scanf("%s", odgovor);
            write(sockfd, odgovor, sizeof(odgovor));
        }
    }
    
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