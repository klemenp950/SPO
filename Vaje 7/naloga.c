#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define M_PI 3.14159265358979323846
#define N_MAX 1e9

// gcc naloga.c -o naloga -lm -lpthread

int P_global = 0;
int N_global = 0;
pthread_t *thread;
int seed;
int N_threads;
pthread_mutex_t mutexP;
int* seeds;

struct timespec myTime;

time_t start, end;


void *thread_function(void *args);

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    pthread_mutex_init(&mutexP, NULL);
    N_threads = atoi(argv[1]);
    printf("N_threads = %d\n", N_threads);
    clock_gettime(CLOCK_REALTIME, &myTime);
    thread = malloc(sizeof(pthread_t) * N_threads);
    seeds = malloc(sizeof(int) * N_threads);

    start = time(NULL);
    for (int i = 0; i < N_threads; i++)
    {   
        seeds[i] = rand();
        pthread_create(&thread[i], NULL, thread_function, &seeds[i]);
    }

    for (int i = 0; i < N_threads; i++)
    {
        pthread_join(thread[i], NULL);
    }

    end = time(NULL);

    pthread_mutex_destroy(&mutexP);

    printf("P = %d\n", P_global);
    printf("N = %d\n", N_global);
    printf("Area = %f\n", ((double)P_global / N_global) * M_PI);
    printf("Time = %ld\n", end - start);
    
    

    return 0;
    free(seeds);
    free(thread);
}

void *thread_function(void *args){
    int seed = *(int*)args;
    double random_number;
    double x;
    double y;
    int local_N = 0;
    int local_P = 0;
    for (;;)
    {
        for (int i = 0; i < 1e6; i++)
        {
            random_number =((double)rand_r(&seed)) / RAND_MAX; 
            y = ((double)rand_r(&seed)) / RAND_MAX;
            x = random_number * M_PI;
            if(y < sin(x)){
                local_P++;
            }
            local_N++;
        }

        pthread_mutex_lock(&mutexP);
        int current_N_global = N_global;
        if(current_N_global >= N_MAX){
            pthread_mutex_unlock(&mutexP);
            return NULL;
        } else {
            N_global += local_N;
            P_global += local_P;
            pthread_mutex_unlock(&mutexP);
        }

        local_N = 0;
        local_P = 0;
    }
}

// void* thread_function(void* args) {
//     double random_number;
//     double x;
//     double y;
//     while(1){
//         random_number =((double)rand_r(&seed)) / RAND_MAX; 
//         y = ((double)rand_r(&seed)) / RAND_MAX;
//         x = random_number * M_PI;
//         if(y < sin(x)){
//             pthread_mutex_lock(&mutexP);
//             P_global++;
//             pthread_mutex_unlock(&mutexP);
//         }
//         pthread_mutex_lock(&mutexP);
//         if(N_global >= 1e9){
//             pthread_mutex_unlock(&mutexP);
//             return NULL;
//         } else {
//             N_global++;
//         }
//         pthread_mutex_unlock(&mutexP);
//     }
// }

