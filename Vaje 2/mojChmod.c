#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define err(mess)	{ fprintf( stderr, "%s\n", mess); exit(0); }
char* vrniDovoljenja(void* st_mode);

int main(int argc, char* argv[]){
    struct stat buf;

    if(argc != 3){
        printf("Usecase: %s mode <filename>\n", argv[0]);
        return 1;
    }

    if(strlen(argv[1]) != 3 && strlen(argv[1]) != 4){
        printf("Usecase: %s u+s <filename>\n", argv[0]);
        printf("or\n");
        printf("Usecase: %s 0777 <filename>\n", argv[0]);
        return 1;
    }

    if(lstat(argv[2], &buf) < 0){
        err("Error opening file.");
    }

    if(strlen(argv[1]) == 3){
        mode_t mode;

        if(strcmp(argv[1], "u+s") == 0){
            mode = buf.st_mode | S_ISUID;
            if(chmod(argv[2], mode) < 0){
                err("Error while changing premissions.");
            }
            printf("New premissions: %s\n", vrniDovoljenja((void*) &mode));
        } else if (strcmp(argv[1], "g+s") == 0){
            mode = buf.st_mode | S_ISGID;
            if(chmod(argv[2], mode) < 0){
                err("Error while changing premissions.");
            }
            printf("New premissions: %s\n", vrniDovoljenja((void*) &mode));
        } else if(strcmp(argv[1], "u-s") == 0){
            mode = buf.st_mode & ~S_ISUID;
            if(chmod(argv[2], mode) < 0){
                err("Error while changing premissions.");
            }
            printf("New premissions: %s\n", vrniDovoljenja((void*) &mode));
        } else if (strcmp(argv[1], "g-s") == 0){
            mode = buf.st_mode & ~S_ISGID;
            if(chmod(argv[2], mode) < 0){
                err("Error while changing premissions.");
            }
            printf("New premissions: %s\n", vrniDovoljenja((void*) &mode));
        } else {
            err("Wrong usage of command. Possible options: \n - u+s\n - u-s\n - g+s\n - g-s")
        }
    } else if (strlen(argv[1]) == 4){
        int mode = strtol(argv[1], NULL, 8);
        if(chmod(argv[2], mode) < 0){
            err("Error while changing premissions.");
        }
        if(lstat(argv[2], &buf) < 0){
            err("Error opening file.");
        }
        printf("New premissions: %s\n", vrniDovoljenja((void*) &buf.st_mode));
    }
    return 0;
}


char* vrniDovoljenja(void* st_mode) {
    mode_t* temp = (mode_t*) st_mode;
    static char dovoljenja[11]; 
    strcpy(dovoljenja, "----------");

    if (S_ISDIR(*temp)) {
        dovoljenja[0] = 'd';
    } else if (S_ISREG(*temp)) {
        dovoljenja[0] = '-';
    } else if (S_ISLNK(*temp)) {
        dovoljenja[0] = 'l';
    } else if (S_ISBLK(*temp)) {
        dovoljenja[0] = 'b';
    } else if (S_ISCHR(*temp)) {
        dovoljenja[0] = 'c';
    } else if (S_ISFIFO(*temp)) {
        dovoljenja[0] = 'p';
    } else if (S_ISSOCK(*temp)) {
        dovoljenja[0] = 's';
    }

    dovoljenja[1] = (*temp & S_IRUSR) ? 'r' : '-';
    dovoljenja[2] = (*temp & S_IWUSR) ? 'w' : '-';
    dovoljenja[3] = (*temp & S_IXUSR) ? 'x' : '-';
    dovoljenja[3] = (*temp & S_ISUID) ? 's' : dovoljenja[3];
    dovoljenja[4] = (*temp & S_IRGRP) ? 'r' : '-';
    dovoljenja[5] = (*temp & S_IWGRP) ? 'w' : '-';
    dovoljenja[6] = (*temp & S_IXGRP) ? 'x' : '-';
    dovoljenja[6] = (*temp & S_ISGID) ? 's' : dovoljenja[6];
    dovoljenja[7] = (*temp & S_IROTH) ? 'r' : '-';
    dovoljenja[8] = (*temp & S_IWOTH) ? 'w' : '-';
    dovoljenja[9] = (*temp & S_IXOTH) ? 'x' : '-';

    return dovoljenja;
}
