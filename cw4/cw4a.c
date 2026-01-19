#define _GNU_SOURCE
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
#include <sys/wait.h>
#include <sys/timerfd.h>
#include <time.h>
#include "cw4a.h"

int main(int argc, char *argv[]) {
    int i;
    int fd;
    char line[200];
    pid_t cpids[MAX_CLIENTS];
    memset(cpids, 0, sizeof(cpids));

    if (argc < 4) {
        fprintf(stderr, "Usage: %s number_of_clients number_of_samples sampling_period processing_delay\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int ncli = atoi(argv[1]);
    if (ncli > MAX_CLIENTS) {
        printf("Number of clients must be below %d\n", (int) MAX_CLIENTS);
        exit(EXIT_FAILURE);
    }

    int nsmp = atoi(argv[2]);
    int udelsmp = atoi(argv[3]);
    int fout = open("server.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fout >= 0);

    fd = shm_open(SHM_CW4_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fd >= 0);
    assert(ftruncate(fd, SHM_LEN) != -1);

    void *mptr = mmap(NULL, SHM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(mptr != MAP_FAILED);
    struct ringbuf *rbuf = (struct ringbuf *) mptr;

    {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        assert(pthread_cond_init(&rbuf->cvar, &attr) == 0);
    }
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        assert(pthread_mutex_init(&rbuf->cvar_lock, &attr) == 0);
    }

    rbuf->magick = 0x12345678;

    for (i = 0; i < ncli; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            static char nr[5];
            sprintf(nr, "%5d", i);
            static char *cargv[] = {"cw4b", nr, NULL, NULL, NULL};
            cargv[2] = argv[2];
            cargv[3] = argv[4];
            execvp("cw4b", cargv);
            printf("I couldn't start cw4b client!\n");
            exit(127);
        } else {
            cpids[i] = pid;
        }
    }

    // Create timerfd
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    assert(tfd != -1);

    struct itimerspec its;
    its.it_value.tv_sec = udelsmp / 1000000;
    its.it_value.tv_nsec = (udelsmp % 1000000) * 1000;
    its.it_interval = its.it_value;

    assert(timerfd_settime(tfd, 0, &its, NULL) != -1);

    unsigned long prev_smptime = 0;

    for (i = 0; i < nsmp; i++) {
        uint64_t expirations;
        read(tfd, &expirations, sizeof(expirations)); // Wait for timer event

        pthread_mutex_lock(&rbuf->cvar_lock);
        int new_head = rbuf->head + 1;
        if (new_head >= BUF_LEN) new_head = 0;

        int overflow = 0;
        for (int j = 0; j < ncli; j++) {
            if (new_head == rbuf->tail[j]) {
                fprintf(stderr, "Client %d buffer overflow!\n", j);
                overflow = 1;
                break;
            }
        }

        if (!overflow) {
            struct timeval tv1;
            gettimeofday(&tv1, NULL);
            memcpy(&rbuf->buf[rbuf->head].tstamp, &tv1, sizeof(tv1));
            rbuf->head = new_head;
            pthread_mutex_unlock(&rbuf->cvar_lock);
            pthread_cond_broadcast(&rbuf->cvar);

            unsigned long smptime = 1000000 * tv1.tv_sec + tv1.tv_usec;
            if (i == 0)
                sprintf(line, "%d, %lu, %lu\n", i, smptime, 0UL);
            else
                sprintf(line, "%d, %lu, %lu\n", i, smptime, smptime - prev_smptime);

            prev_smptime = smptime;
            assert(write(fout, line, strlen(line)) > 0);
            sync();
        } else {
            pthread_mutex_unlock(&rbuf->cvar_lock);
        }
    }

    printf("waiting for children");
    for (i = 0; i < ncli; i++) {
        if (cpids[i]) waitpid(cpids[i], 0, 0);
    }

    close(tfd);
    munmap(mptr, SHM_LEN);
    shm_unlink(SHM_CW4_NAME);
    close(fout);
}

