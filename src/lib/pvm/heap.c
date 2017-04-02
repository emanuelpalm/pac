#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    int fd = open("tmp.mmap", O_RDWR | O_CREAT);
    if (fd < 0) {
        puts(strerror(errno));
        return 1;
    }
    for (int i = 0; i < 64; ++i) {
        uint64_t len = 1UL << i;
        printf("Mapping %llu bytes of memory ... ", len);
        uint8_t* m = mmap(NULL, len, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_NORESERVE, fd, 0);
        if (m == MAP_FAILED) {
            printf("failed (%s).\n", strerror(errno));
        } else {
            puts("succeeded.");
            munmap(m, len);
        }
    }
    puts("Done.");
    close(fd);

    return 0;
}

