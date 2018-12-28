#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/file.h>
#include <pthread.h>
#include <wait.h>
#include <memory.h>
#include <string.h>

#define BLOCK_SIZE 512
#define BUFFER_SIZE 8

#define P(x)         \
    {                \
        sem_wait(x); \
    }
#define V(x)         \
    {                \
        sem_post(x); \
    }

sem_t *sem1, *sem2;
pid_t pid1, pid2;
int shmid_b, shmid_s, shmid_e, shmid_sem1, shmid_sem2, shmid_status;
int *start, *end, *status;
char *buffer;
int main(int argc, char const *argv[])
{
    if (argc <= 2)
    {
        return 0;
    }
    FILE *input = fopen(argv[1], "rb");
    FILE *output = fopen(argv[2], "wb");
    shmid_b = shmget(IPC_PRIVATE, BUFFER_SIZE * (BLOCK_SIZE + 1) * sizeof(char), IPC_CREAT | 0666);
    shmid_s = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid_e = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid_status = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid_sem1 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    shmid_sem2 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    buffer = (char *)shmat(shmid_b, 0, 0);
    start = (int *)shmat(shmid_s, 0, 0);
    end = (int *)shmat(shmid_e, 0, 0);
    status = (int *)shmat(shmid_status, 0, 0);
    sem1 = (sem_t *)shmat(shmid_sem1, 0, 0);
    sem2 = (sem_t *)shmat(shmid_sem2, 0, 0);
    *start = 0;
    *end = 0;
    sem_init(sem1, 1, 0);
    sem_init(sem2, 1, BUFFER_SIZE - 1);
    *status = 1;
    if ((pid1 = fork()) == 0)
    {
        char block[BLOCK_SIZE + 1];
        while (*status || *start != *end)
        {
            P(sem1);
            memcpy(block, buffer + *start * (BLOCK_SIZE + 1), (BLOCK_SIZE + 1) * sizeof(char));
            fwrite(block, 1, strlen(block), output);
            *start = (*start + 1) % BUFFER_SIZE;
            V(sem2);
        }
        return 1;
    }
    else if ((pid2 = fork()) == 0)
    {
        int c;
        char block[BLOCK_SIZE + 1];
        c = fread(block, 1, BLOCK_SIZE, input);
        block[c] = '\0';
        while (c == BLOCK_SIZE)
        {
            P(sem2);
            memcpy(buffer + *end * (BLOCK_SIZE + 1), block, (BLOCK_SIZE + 1) * sizeof(char));
            *end = (*end + 1) % BUFFER_SIZE;
            V(sem1);
            c = fread(block, 1, BLOCK_SIZE, input);
            block[c] = '\0';
        }
        P(sem2);
        memcpy(buffer + *end * (BLOCK_SIZE + 1), block, (BLOCK_SIZE + 1) * sizeof(char));
        *end = (*end + 1) % BUFFER_SIZE;
        V(sem1);
        *status = 0;
        return 2;
    }
    int pid_end;
    wait(&pid_end);
    wait(&pid_end);
    sem_destroy(sem1);
    sem_destroy(sem2);
    fclose(input);
    fclose(output);
    shmctl(shmid_b, IPC_RMID, 0);
    shmctl(shmid_s, IPC_RMID, 0);
    shmctl(shmid_e, IPC_RMID, 0);
    shmctl(shmid_status, IPC_RMID, 0);
    shmctl(shmid_sem1, IPC_RMID, 0);
    shmctl(shmid_sem2, IPC_RMID, 0);
    return 0;
}