#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MAX_COMMAND_LENGTH 4096  

void compile_c_files(const char *dir_path) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        const char *file_name = entry->d_name;
        const char *ext = strrchr(file_name, '.');

        if (ext != NULL && strcmp(ext, ".c") == 0) {
            char command[MAX_COMMAND_LENGTH];
            char output_file[MAX_PATH];
            
            strncpy(output_file, file_name, ext - file_name);
            output_file[ext - file_name] = '\0';  // Null-terminate
            
            snprintf(command, MAX_COMMAND_LENGTH, "gcc -o %s/%s %s/%s", dir_path, output_file, dir_path, file_name);

            printf("Compiling: %s\n", command);
            int ret = system(command);
            if (ret != 0) {
                fprintf(stderr, "Error: Failed to compile %s\n", file_name);
            }
        }
    }

    closedir(dp);
}

int main() {
    char *dir_path = ".";

    size_t len = strlen(dir_path);
    if (len > 0 && dir_path[len - 1] == '\n') {
        dir_path[len - 1] = '\0';
    }

    compile_c_files(dir_path);

    return 0;
}
