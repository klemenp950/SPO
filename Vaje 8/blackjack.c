#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *karte[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
const int vrednosti_kart[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
int igralec = 0;
int hisa = 0;

void blackjack();
int get_random();
void deal_initial_cards();
void deal_to_player();
void deal_initial_cards_house();
void deal_to_house();

int main(){
    while (1)
    {
        printf("-----------------------------------------------------\n");
        blackjack();
    }
}

void blackjack(){
    srand(time(NULL));
    deal_initial_cards();
    deal_initial_cards_house();
    while(igralec <= 21) {
        printf("Še ena karta? (da/ne)\n");
        char odgovor[3];
        scanf("%s", odgovor);
        if(strcmp(odgovor, "da") == 0) {
            deal_to_player();
            if(igralec > 21) {
                printf("BUST: %d\n", igralec);
                return;
            }
        } else {
            break;
        }
    }
    while(hisa < 17) {
        deal_to_house();
    }

    if(hisa > 21) {
        printf("Hisa bust: %d\n", hisa);
        printf("WIN!\n");
        return;
    }

    if(igralec > hisa) {
        printf("WIN!\n");
        printf("Tvoja vrednost: %d\n", igralec);
        printf("Vrednost hiše: %d\n", hisa);
    } else if(igralec == hisa) {
        printf("SPLIT!\n");
        printf("Tvoja vrednost: %d\n", igralec);
        printf("Vrednost hiše: %d\n", hisa);
    } else {
        printf("LOSS!\n");
        printf("Tvoja vrednost: %d\n", igralec);
        printf("Vrednost hiše: %d\n", hisa);
    }
}

int get_random() {
    return rand() % 13;
}

void deal_initial_cards() {
    int first_number = get_random();
    int second_number = get_random();
    igralec = vrednosti_kart[first_number] + vrednosti_kart[second_number];
    printf("Tvoja prva karta: %s\n", karte[first_number]);
    printf("Tvoja druga karta: %s\n", karte[second_number]);
    printf("Tvoja trenutna vrednost: %d\n", igralec);
}

void deal_to_player(){
    int number = get_random();
    igralec += vrednosti_kart[number];
    printf("Karta: %s\n", karte[number]);
    printf("Trenutna vrednost: %d\n", igralec);
    if(igralec == 21) {
        printf("BLACKJACK!\n");
        exit(1);
    }
}

void deal_initial_cards_house(){
    int first_number = get_random();
    int second_number = get_random();
    hisa = vrednosti_kart[first_number] + vrednosti_kart[second_number];
    printf("Prva karta hiše: %s\n", karte[first_number]);
    printf("Druga karta hiše: %s\n", karte[second_number]);
    printf("Trenutna vrednost hiše: %d\n", hisa);
}

void deal_to_house(){
    int number = get_random();
    hisa += vrednosti_kart[number];
    printf("Karta za hišo: %s\n", karte[number]);
    printf("Trenutna vrednost hiše: %d\n", hisa);
    if(hisa == 21) {
        printf("HIŠA BLACKJACK!\n");
        exit(1);
    }
}