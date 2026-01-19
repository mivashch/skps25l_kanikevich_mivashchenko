// SKPS 2022 Ex 4, demo code by WZab – modified for active wait control via file
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "cw4a.h"

volatile int dummy = 0; // Used to simulate processing delay

int get_wait_mode() {
    FILE *f = fopen("/tmp/cw4b_mode.txt", "r");
    if (!f) return 0;
    int mode = 0;
    fscanf(f, "%d", &mode);
    fclose(f);
    printf("Waiting mode %d\n", mode);
    return mode;
}

int main(int argc, char *argv[]) {
    int smpnum;
    unsigned long smptime, deliverytime;
    int fd;
    double tdel;
    int ncli;
    int nsmp;
    int ndel;
    int fout;

    ncli = atoi(argv[1]);
    nsmp = atoi(argv[2]);
    ndel = atoi(argv[3]);

    printf("Client: %d, nsmp=%d, del=%d\n", ncli, nsmp, ndel);

    // Check wait mode
    int wait_mode = get_wait_mode();
    int active_wait = (wait_mode == 2) || (wait_mode == 1 && ncli == 0);
    if (active_wait) {
        printf("Client %d: active waiting enabled\n", ncli);
    }

    // Create report file
    char fname[20];
    sprintf(fname, "cli_%d.txt", ncli);
    fout = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fout >= 0);

    // Open shared memory
    fd = shm_open(SHM_CW4_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    assert(fd >= 0);

    // Map the shared memory
    void *mptr = mmap(NULL, SHM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(mptr != MAP_FAILED);
    struct ringbuf *rbuf = (struct ringbuf *) mptr;

    // Check initialization
    assert(rbuf->magick == 0x12345678);

    // Main loop
    smpnum = 0;
    while (smpnum < nsmp) {
        int j;
        char line[200];

        if (active_wait) {
            while (1) {
                pthread_mutex_lock(&rbuf->cvar_lock);
                if (rbuf->head != rbuf->tail[ncli]) {
                    break;
                }
                pthread_mutex_unlock(&rbuf->cvar_lock);
            }
        } else {
            pthread_mutex_lock(&rbuf->cvar_lock);
            if (rbuf->head == rbuf->tail[ncli]) {
                pthread_cond_wait(&rbuf->cvar, &rbuf->cvar_lock);
            }
        }

        struct timeval tv1, tv2;
        gettimeofday(&tv1, NULL);
        tv2 = rbuf->buf[rbuf->tail[ncli]].tstamp;
        rbuf->tail[ncli]++;
        if (rbuf->tail[ncli] == BUF_LEN) rbuf->tail[ncli] = 0;
        pthread_mutex_unlock(&rbuf->cvar_lock);

        smptime = 1000000 * tv2.tv_sec + tv2.tv_usec;
        deliverytime = 1000000 * tv1.tv_sec + tv1.tv_usec;
        int tdel = deliverytime - smptime;
        printf("Sample %d, client %d, delivery time: %d\n", smpnum, ncli, tdel);
        sprintf(line, "%d, %lu, %lu, %d\n", smpnum, smptime, deliverytime, tdel);
        assert(write(fout, line, strlen(line)) > 0);
        sync();
        smpnum++;

        // Simulate data processing delay
        for (j = 0; j < ndel; j++)
            dummy++;
    }

    munmap(mptr, SHM_LEN);
    shm_unlink(SHM_CW4_NAME);
    close(fout);
    return 0;
}
