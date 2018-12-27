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
#define P(x)          \
    {                 \
        sem_wait(x); \
    }
#define V(x)          \
    {                 \
        sem_post(x); \
    }

sem_t *sem1, *sem2, *sem3;
pid_t pid1, pid2;
int shmid_b, shmid_s, shmid_e, shmid_sem1, shmid_sem2, shmid_sem3;
int *start, *end;
char* buffer;
int main(int argc, char const *argv[])
{
    if (argc <= 2)
    {
        return 0;
    }
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    shmid_b = shmget(IPC_PRIVATE, 8, IPC_CREAT | 0666);
    shmid_s = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid_e = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid_sem1 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    shmid_sem2 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    shmid_sem3 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    buffer = (char*)shmat(shmid_b, 0, 0);
    start = (int*)shmat(shmid_s, 0, 0);
    end = (int*)shmat(shmid_e, 0, 0);
    sem1 = (sem_t*)shmat(shmid_sem1, 0, 0);
    sem2 = (sem_t*)shmat(shmid_sem2, 0, 0);
    sem3 = (sem_t*)shmat(shmid_sem3, 0, 0);
    *start = 0;
    *end = 0;
    sem_init(sem1, 1, 0);
    sem_init(sem2, 1, 8);
    sem_init(sem3, 1, 1);
    if ((pid1 = fork()) == 0)
    {
        while (1)
        {
            P(sem1);
            if (*(buffer + *start) == '#')
            {
                return 1;
            }
            fprintf(output,"%c",*(buffer + *start));
            P(sem3);
            printf("1:%c\n", *(buffer + *start));
            V(sem3);
            *start = (*start + 1) % 8;
            V(sem2);
        }
    }
    else if ((pid2 = fork()) == 0)
    {
        char c;
        fscanf(input,"%c", &c);
        while (c != '#')
        {
            P(sem2);
            *(buffer + *end) = c;
            P(sem3);
            printf("2:%c\n", *(buffer + *end));
            V(sem3);
            *end = (*end + 1) % 8;
            V(sem1);
            fscanf(input, "%c", &c);
        }
        P(sem2);
        *(buffer + *end) = c;
        *end = (*end + 1) % 8;
        V(sem1);
        return 2;
    }
    int status;
    wait(&status);
    printf("end:%d\n", status);
    wait(&status);
    printf("end:%d\n", status);
    sem_destroy(sem1);
    sem_destroy(sem2);
    sem_destroy(sem3);
    fclose(input);
    fclose(output);
    shmctl(shmid_b, IPC_RMID, 0);
    shmctl(shmid_s, IPC_RMID, 0);
    shmctl(shmid_e, IPC_RMID, 0);
    shmctl(shmid_sem1, IPC_RMID, 0);
    shmctl(shmid_sem2, IPC_RMID, 0);
    shmctl(shmid_sem3, IPC_RMID, 0);
    return 0;
}