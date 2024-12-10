#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

const char *karte[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
const int vrednosti_kart[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void deal_to_house(int *hisa, int sockfd);
void deal_initial_cards_house(int *hisa, int sockfd);
void deal_to_player(int *igralec, int sockfd);
void deal_initial_cards(int *igralec, int sockfd);
void blackjack(int sockfd);

int main(){
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

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
        int temp = Accept(sockfd, NULL, NULL);
        blackjack(temp);
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

void blackjack(int sockfd){
    int igralec = 0;
    int hisa = 0;
    srand(time(NULL));
    deal_initial_cards(&igralec, sockfd);
    deal_initial_cards_house(&hisa, sockfd);

    while(igralec < 21) {
        printf("Še ena karta? (da/ne)\n");
        write(sockfd, "Še ena karta? (H/S)\n", strlen("Še ena karta? (H/S)\n"));
        char odgovor[10] = {0}; // Initialize the buffer to zero
        int bytes_read = read(sockfd, odgovor, sizeof(odgovor) - 1); // Leave space for null terminator
        if (bytes_read < 0) {
            perror("read");
            close(sockfd);
            return;
        }
        odgovor[bytes_read] = '\0'; // Null-terminate the received string
        sleep(1);
        if(strcmp(odgovor, "H") == 0) {
            deal_to_player(&igralec, sockfd);
            if(igralec > 21) {
                char buffer[100];
                snprintf(buffer, sizeof(buffer), "BUST: %2d\n", igralec);
                write(sockfd, buffer, strlen(buffer));
                close(sockfd);
                return;
            }
        } else if (strcmp(odgovor, "S") == 0) {
            break;
        }
    }

    while(hisa < 17) {
        deal_to_house(&hisa, sockfd);
    }

    if(hisa > 21) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Hisa bust: %2d\n", hisa);
        write(sockfd, buffer, strlen(buffer));
        write(sockfd, "WIN\n", strlen("WIN\n"));
        close(sockfd);
        return;
    }

    if(igralec > hisa) {
        write(sockfd, "WIN\n", strlen("WIN\n"));
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Tvoja vrednost: %d\n", igralec);
        write(sockfd, buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "Vrednost hiše: %2d\n", hisa);
        write(sockfd, buffer, strlen(buffer));
    } else if(igralec == hisa) {
        write(sockfd, "SPLIT\n", strlen("SPLIT\n"));
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Tvoja vrednost: %d\n", igralec);
        write(sockfd, buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "Vrednost hiše: %d\n", hisa);
        write(sockfd, buffer, strlen(buffer));
    } else {
        write(sockfd, "LOSS!\n", strlen("LOSS!\n"));
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Tvoja vrednost: %d\n", igralec);
        write(sockfd, buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "Vrednost hiše: %2d\n", hisa);
        write(sockfd, buffer, strlen(buffer));
    }
    close(sockfd);
}


int get_random() {
    return rand() % 13;
}

void deal_initial_cards(int *igralec, int sockfd){ 
    int first_number = get_random();
    int second_number = get_random();
    *igralec = vrednosti_kart[first_number] + vrednosti_kart[second_number];
    char buffer[100];
    
    snprintf(buffer, sizeof(buffer), "Tvoja prva karta: %s\n", karte[first_number]);
    write(sockfd, buffer, strlen(buffer));
    printf("Tvoja prva karta: %s\n", karte[first_number]);

    char buffer2[100];
    
    snprintf(buffer2, sizeof(buffer2), "Tvoja druga karta: %s\n", karte[second_number]);
    write(sockfd, buffer2, strlen(buffer2));
    printf("Tvoja druga karta: %s\n", karte[second_number]);
    
    char buffer3[100];

    snprintf(buffer3, sizeof(buffer3), "Tvoja trenutna vrednost: %d\n", *igralec);
    write(sockfd, buffer3, strlen(buffer3));
    printf("Tvoja trenutna vrednost: %d\n", *igralec);
}

void deal_to_player(int *igralec, int sockfd){
    int number = get_random();
    *igralec += vrednosti_kart[number];
    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "Karta: %s\n", karte[number]);
    write(sockfd, buffer, strlen(buffer));
    printf("Karta: %s\n", karte[number]);

    snprintf(buffer, sizeof(buffer), "Trenutna vrednost: %d\n", *igralec);
    write(sockfd, buffer, strlen(buffer));
    printf("Trenutna vrednost: %d\n", *igralec);
    if(*igralec == 21) {
        snprintf(buffer, sizeof(buffer), "BLACKJACK!\n");
        write(sockfd, buffer, strlen(buffer));
        printf("BLACKJACK!\n");
        close(sockfd);
    }
}

void deal_initial_cards_house(int *hisa, int sockfd){
    int first_number = get_random();
    int second_number = get_random();
    *hisa = vrednosti_kart[first_number] + vrednosti_kart[second_number];

    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "Prva karta hiše: %s\n", karte[first_number]);
    write(sockfd, buffer, strlen(buffer));
    printf("Prva karta hiše: %s\n", karte[first_number]);

    snprintf(buffer, sizeof(buffer), "Druga karta hiše: %s\n", karte[second_number]);
    write(sockfd, buffer, strlen(buffer));
    printf("Druga karta hiše: %s\n", karte[second_number]);

    snprintf(buffer, sizeof(buffer), "Trenutna vrednost hiše: %d\n", *hisa);
    write(sockfd, buffer, strlen(buffer));
    printf("Trenutna vrednost hiše: %d\n", *hisa);
}

void deal_to_house(int *hisa, int sockfd){
    int number = get_random();
    *hisa += vrednosti_kart[number];

    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "Karta za hišo: %s\n", karte[number]);
    write(sockfd, buffer, strlen(buffer));
    printf("Karta za hišo: %s\n", karte[number]);

    snprintf(buffer, sizeof(buffer), "Trenutna vrednost hiše: %d\n", *hisa);
    write(sockfd, buffer, strlen(buffer));
    printf("Trenutna vrednost hiše: %d\n", *hisa);

    if(*hisa == 21) {
        snprintf(buffer, sizeof(buffer), "HIŠA BLACKJACK!\n");
        write(sockfd, buffer, strlen(buffer));
        printf("HIŠA BLACKJACK!\n");
        close(sockfd);
    }
}