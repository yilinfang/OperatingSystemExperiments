#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

int pipefd[2];
pid_t pid1, pid2;

void func1(int sig_num)
{
    if(sig_num == SIGINT)
    {
        return;
    }
    else if(sig_num == SIGUSR1)
    {
        printf("Child Process 1 is Killed by Parent!\n");
        close(pipefd[1]);
        exit(0);
    }
    else
    {
        printf("Receive invalid signal!\n");
        return;
    }
}

void func2(int sig_num)
{
    if(sig_num == SIGINT)
    {
        return;
    }
    else if(sig_num == SIGUSR1)
    {
        printf("Child Process 2 is Killed by Parent!\n");
        close(pipefd[0]);
        exit(0);
    }
    else
    {
        printf("Receive invalid signal!\n");
        return;
    }
}

void func3(int sig_num)
{
    if(sig_num == SIGINT)
    {
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR1);
        int status;
        wait(&status);
        wait(&status);
        printf("Parent Process is Killed!\n");
        exit(0);
    }
    else
    {
        printf("Receive invalid signal!\n");
        return;
    }
}


int main(int argc, char const *argv[])
{
    int x = 1;
    pipe(pipefd);
    signal(SIGINT, func3);
    if((pid1 = fork()) == 0)
    {
        signal(SIGINT, func1);
        signal(SIGUSR1, func1);
        while(1)
        {
            write(pipefd[1], &x, sizeof(int));
            printf("I send you %d times.\n", x);
            x++;
            sleep(1);
        }
    }
    else if((pid2 = fork()) == 0)
    {
        signal(SIGINT, func2);
        signal(SIGUSR1, func2);
        while(1)
        {
            int n;
            read(pipefd[0], &n, sizeof(int));
            printf("I receive %d times.\n", n);
        }
    }
    while(1);
    return 0;
}

