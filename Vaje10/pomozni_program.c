#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    char buffer[256];

    fd = open("/dev/gonilnik", O_RDWR);
    if (fd == -1) {
        perror("Napaka pri odpiranju ");
        return EXIT_FAILURE;
    }

    while (read(fd, buffer, sizeof(buffer)) > 0) {
        printf("%s", buffer);
    }
    write(fd, "Konec\n", 6);

    close(fd);
    return EXIT_SUCCESS;
}