#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#define P(x)          \
    {                 \
        sem_wait(&x); \
    }
#define V(x)          \
    {                 \
        sem_post(&x); \
    }
sem_t sem1;
sem_t sem2;
pthread_t subp1;
pthread_t subp2;
int sum = 0;
void *thread1(void *temp1)
{
    while (1)
    {
        P(sem1)
        printf("thread1:sum=%d\n", sum);
        V(sem2);
    };
};
void *thread2(void *temp2)
{
    for (int i = 1; i <= 100; i++)
    {
        P(sem2);
        printf("thread2:i=%d\n", i);
        sum += i;
        V(sem1);
    };
    P(sem2); //等待打印工作完成
    pthread_cancel(subp1); //结束线程1
};
int main()
{
    sem_init(&sem1, 0, 0);                       //初始化sem1
    sem_init(&sem2, 0, 1);                       //初始化sem2
    pthread_create(&subp1, NULL, thread1, NULL); //创建thread1
    pthread_create(&subp2, NULL, thread2, NULL); //创建thread2
    pthread_join(subp1, NULL);                   //等待线程1
    pthread_join(subp2, NULL);                   //等待线程2
    sem_destroy(&sem1);                          //销毁sem1
    sem_destroy(&sem2);                          //销毁sem2
    return 0;
}