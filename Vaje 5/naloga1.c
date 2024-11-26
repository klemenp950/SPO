#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

unsigned int unslept_time = 0;

void handle_sigint(int signo) {
    printf("\nPrejel sem signal %d.\n", signo);
    unslept_time = alarm(0); 
    printf("unslept_time = %u.\n", unslept_time);
    alarm(unslept_time); 
}

void handle_alarm(int signo) {
    static int phase = 1;

    if (phase == 1) {
        printf("Alarm v casu t = 5 ...\n");
        printf("Ignoriram signal SIGINT ...\n");
        signal(SIGINT, SIG_IGN); // ignoriraj SIGINT
        alarm(5);
    } else if (phase == 2) {
        printf("Alarm v casu t = 10 ...\n");
        printf("Privzeta akcija za SIGINT ...\n");
        signal(SIGINT, SIG_DFL); // nastavi privzeto akcijo za SIGINT
        alarm(5);
    } else if (phase == 3) {
        printf("Alarm v casu t = 15 ...\n");
        exit(0); // zaključek programa
    }

    phase++;
}

int main() {
    printf("Lovim signal SIGINT ...\n");

    signal(SIGINT, handle_sigint);

    signal(SIGALRM, handle_alarm);

    alarm(5);

    while (1) {
        pause(); 
    }

    return 0;
}
