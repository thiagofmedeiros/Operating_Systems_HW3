/*
*   Name: Thiago André Ferreira Medeiros
*/

/*
 * 3 runs of mutex had the following times:
 *  run 1:
 *          Time used is sec: 0, usec 666330
 *          System Time used is sec: 0, usec 1000
 *
 *  run 2:
 *          Time used is sec: 0, usec 673074
 *          System Time used is sec: 0, usec 0
 *
 *  run 3:
 *          Time used is sec: 0, usec 612624
 *          System Time used is sec: 0, usec 1001
 *
 * The medium total time for mutex is : 651343
 *
 * 3 runs of semaphores for 2 threads had the following times:
 *  run 1:
 *          Time used is sec: 0, usec 482937
 *          System Time used is sec: 0, usec 412932
 *
 *  run 2:
 *          Time used is sec: 0, usec 391142
 *          System Time used is sec: 0, usec 305579
 *
 *  run 3:
 *          Time used is sec: 0, usec 296120
 *          System Time used is sec: 0, usec 237099
 *
 * The medium total time for semaphores is : 708603
 *
 * Semaphores spend almost an equal amount of time in user mode and kernel mode and the switch
 * between contexts seems to make it run slower than the mutex solution
 *
 */

#define _REENTRANT
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
/* Tested by LOH 2/1/22. */
/* compile with gcc -ofname main.c -lpthread */
/* Solve with semaphores */

struct rusage mytiming;
struct timeval mytimeval;

struct shared_dat
{
    int value;     /* shared variable to store result*/
};

struct shared_dat  *counter;
int getpid();
int temp;
sem_t mutex;
int mytot=0, jumps=0;

/****************************************************************
* This function increases the value of shared variable "counter"
  by one 3000000 times
****************************************************************/
void * thread1(void *arg)
{
    int line = 0;
    while (line < 3000000)
    {
        // Wait for semaphore
        sem_wait (&mutex);

        if (counter->value % 100 == 0 &&
            line <= 3000000 - 100) {
            line += 100;
            mytot += 100;
            counter->value = counter->value + 100;
            jumps += 1;
        } else {
            line++;
            mytot++;
            counter->value = counter->value + 1;
            counter->value = counter->value * 2;
            counter->value = counter->value / 2;
        }

        // signal semaphore
        sem_post (&mutex);
    }
    // Ignore if you like.  Your counter value is the shared variable
    // when done.  mytot should be 3,000,000.  Jumps is 0 to 30,0000.
    printf("from process1 counter  =  %d, mytot %d, jumps %d \n", counter->value, mytot, jumps);
    return(NULL);
}


/****************************************************************
This function increases the value of shared variable "counter"
by one 3000000 times
****************************************************************/
void * thread2(void *arg)
{
    int line = 0;
    int count;
    while (line < 3000000)
    {
        // Wait for semaphore
        sem_wait (&mutex);

        line++;

        count = 0;
        /* Critical Section */
        counter->value = counter->value + 1;
        counter->value = counter->value * 2;
        counter->value = counter->value / 2;

        // signal semaphore
        sem_post(&mutex);

    }
    printf("from process2 counter = %d\n", counter->value);
    return(NULL);
}

/****************************************************************
This function increases the value of shared variable "counter"
by one 3000000 times
 Could use thread2 function on a third thread, but the printing
 would be from the wrong thread
****************************************************************/
void * thread3(void *arg)
{
    int line = 0;
    int count;
    while (line < 3000000)
    {
        // Wait for semaphore
        sem_wait (&mutex);

        line++;

        count = 0;
        /* Critical Section */
        counter->value = counter->value + 1;
        counter->value = counter->value * 2;
        counter->value = counter->value / 2;

        // signal semaphore
        sem_post(&mutex);

    }
    printf("from process3 counter = %d\n", counter->value);
    return(NULL);
}

/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main()
{
    int             r=0;
    int 		i;
    pthread_t	tid1[1];     /* process id for thread 1 */
    pthread_t	tid2[1];     /* process id for thread 2 */
    pthread_t	tid3[1];     /* process id for thread 3 */
    pthread_attr_t	attr[1];     /* attribute pointer array */

    // Init Semaphore
    sem_init(&mutex, 0, 1);

    counter = (struct shared_dat *) malloc(sizeof(struct shared_dat));

    /* initialize shared memory to 0 */
    counter->value = 0 ;
    printf("1 - I am here %d in pid %d\n",r,getpid());

    fflush(stdout);
    /* Required to schedule thread independently.
    Otherwise use NULL in place of attr. */
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);  /* system-wide contention */

    /* end to schedule thread independently */

/* Create the threads */

    pthread_create(&tid3[0], &attr[0], thread3, NULL);
    pthread_create(&tid2[0], &attr[0], thread2, NULL);
    pthread_create(&tid1[0], &attr[0], thread1, NULL);


/* Wait for the threads to finish */
    pthread_join(tid3[0], NULL);
    pthread_join(tid2[0], NULL);
    pthread_join(tid1[0], NULL);



    printf("from parent counter  =  %d\n", counter->value);
    getrusage(RUSAGE_SELF, &mytiming);
    printf("Time used is sec: %d, usec %d\n",mytiming.ru_utime.tv_sec,
           mytiming.ru_utime.tv_usec);
    printf("System Time used is sec: %d, usec %d\n",mytiming.ru_stime.tv_sec,
           mytiming.ru_stime.tv_usec);
    printf("---------------------------------------------------------------------------\n");
    printf("\t\t	End of simulation\n");

    exit(0);

}