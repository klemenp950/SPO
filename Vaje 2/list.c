#include <stdio.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

struct stat buf;
struct dirent *element;

char* vrniDovoljenja(void* st_mode);

int main(int argc, char argv[]){
    DIR *dir;

    dir = opendir(".");

    if(argc != 1){
        printf("Usage: ./list\n");
        return 1;
    }

    if(!dir){
        err("Unable to open directory.");
    }

    while ((element = readdir(dir)) != NULL)
    {
        if(lstat(element->d_name, &buf) < 0){
            err("Napaka pri odprianju elementa direktorija.");
        }

        printf("%s %ld %s\n", vrniDovoljenja((void*) &buf.st_mode), buf.st_nlink, element->d_name);
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
