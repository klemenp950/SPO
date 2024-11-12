#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SENTENCE "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
#define FIVE_MILLION 5000000
#define buffer_size1 10
#define buffer_size2 100

FILE *file1;
FILE *file2;
char filename_file1[] = "izhod1";
char filename_file2[] = "izhod2";
char buffer1[buffer_size1];
char buffer2[buffer_size2];    

time_t start, end;

int main(){
    // Pisanje v prvo datoteko, buffer size 10
    file1 = fopen(filename_file1, "w");
    if (!file1)
    {
        printf("Error opening file %s\n", filename_file1);
        return 1;
    }
    
    setvbuf(file1, buffer1, _IOFBF, buffer_size1);

    start = time(NULL);
    for(int i = 0; i < FIVE_MILLION; i++){
        fputs(SENTENCE, file1);
    }
    end = time(NULL);
    printf("%s writen in %ld seconds.\n", filename_file1, end - start);
    fclose(file1);

    // Pisanje v drugo datoteko, buffer size 100
    file2 = fopen(filename_file2, "w");
    if (!file2)
    {
        printf("Error opening file %s\n", filename_file2);
        return 1;
    }
    setvbuf(file2, buffer2, _IOFBF, buffer_size2);

    start = time(NULL);
    for(int i = 0; i < FIVE_MILLION; i++){
        fputs(SENTENCE, file2);
    }
    end = time(NULL);
    printf("%s writen in %ld seconds.\n", filename_file2, end - start);
    fclose(file2);

    return 0;
}
