#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LENGTH 1024

FILE *file;
char input[MAX_LENGTH];
int printFileContent(const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        printf("Usage:\n");
        printf("Append line: %s <filename> -a\n", argv[0]);
        printf("Remove line: %s <filename> -d <line_number>\n", argv[0]);
        printf("Insert line: %s <filename> -i <line_number>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[2], "-a") == 0) {
        file = fopen(argv[1], "a");
        if (!file) {
            printf("Error opening file %s\n", argv[1]);
            return 1;
        }

        while (fgets(input, MAX_LENGTH, stdin) != NULL) {
            setvbuf(file, NULL, _IOLBF, MAX_LENGTH);
            fprintf(file, "%s", input);
            printf("%s", input);
        }
        fclose(file);
        printFileContent(argv[1]);

    } else if (strcmp(argv[2], "-d") == 0) {
        int line_number = atoi(argv[3]);
        struct stat statbuf;
        if (stat(argv[1], &statbuf) != 0) {
            perror("Error getting file stats");
            return 1;
        }

        int new_fd = open("new_file", O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
        FILE *old_file = fopen(argv[1], "r");
        FILE *new_file = fdopen(new_fd, "a");

        if (!old_file || !new_file) {
            perror("Error opening files");
            return 1;
        }

        for (int i = 1; i < line_number; i++) {
            if (fgets(input, MAX_LENGTH, old_file) != NULL) {
                fputs(input, new_file);
            }
        }

        fgets(input, MAX_LENGTH, old_file);

        while (fgets(input, MAX_LENGTH, old_file) != NULL) {
            fputs(input, new_file);
        }

        fclose(old_file);
        fclose(new_file);
        unlink(argv[1]);
        rename("new_file", argv[1]);

        printFileContent(argv[1]);

    } else if (strcmp(argv[2], "-i") == 0) {
        fgets(input, MAX_LENGTH, stdin);
        char *line = strdup(input);
        int line_number = atoi(argv[3]);
        struct stat statbuf;

        if (stat(argv[1], &statbuf) < 0) {
            perror("Error getting file stats");
            return 1;
        }

        int new_fd = open("new_file", O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
        FILE *old_file = fopen(argv[1], "r");
        FILE *new_file = fdopen(new_fd, "a");

        if (!old_file || !new_file) {
            perror("Error opening files");
            return 1;
        }

        for (int i = 1; i < line_number; i++) {
            if (fgets(input, MAX_LENGTH, old_file) != NULL) {
                fputs(input, new_file);
            }
        }

        fputs(line, new_file);

        while (fgets(input, MAX_LENGTH, old_file) != NULL) {
            fputs(input, new_file);
        }

        fclose(old_file);
        fclose(new_file);
        free(line);
        unlink(argv[1]);
        rename("new_file", argv[1]);

        printFileContent(argv[1]);
    }

    return 0;
}

int printFileContent(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file for printing");
        return 1;
    }

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
    return 0;
}
