// compile with: gcc dining_philosophers.c -o dining_philosophers -lpthread


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define THINK_TIME	100000
#define EAT_TIME	100000
#define BETWEEN_CHOPSTICK_TIME	100000
#define P_C_COUNT	5

pthread_t p[P_C_COUNT];
pthread_mutex_t c[P_C_COUNT];
int deadlockDetector[P_C_COUNT];
struct timespec think = {0,THINK_TIME};
struct timespec eat = {0,EAT_TIME};
struct timespec btw = {0,BETWEEN_CHOPSTICK_TIME};
//think.tv_sec = 0;
//eat.tv_sec = 0;
//btw.tv_sec = 0;
//think.tv_nsec = THINK_TIME;
//eat.tv_nsec = EAT_TIME;
//btw.tv_nsec = BETWEEN_CHOPSTICK_TIME;

void *dine(void *thread)
   {
   while(1){
	   int n = (intptr_t) thread;
	   int second_chopstick = (n+1)%P_C_COUNT;
	   printf("Philosopher %d is thinking\n",n);
	   if(nanosleep(&think,NULL) < 0){
	       printf("Nanosleep call failed\n");
	       return (void*)-1;
	   }
	   printf("Philosopher %d is grabbing a chopstick\n",n);
	   pthread_mutex_lock(&c[n]);
	   deadlockDetector[n] = n; // philosopher grabs chopstick of same num initially
	   if(nanosleep(&btw,NULL)){
	       printf("Nanosleep call failed\n");
	       return (void*)-1;
	   }
	   printf("Philosopher %d is attempting to grab a second chopstick\n",n); 
	   pthread_mutex_lock(&c[second_chopstick]);
	   deadlockDetector[second_chopstick] = n;
	   printf("Philosopher %d is eating\n",n);
	   if(nanosleep(&eat,NULL)){
	       printf("Nanosleep call failed\n");
	       return (void*)-1;
	   }
	   pthread_mutex_unlock(&c[n]);
	   deadlockDetector[n] = -1;
	   pthread_mutex_unlock(&c[second_chopstick]);
	   deadlockDetector[second_chopstick] = -1;

   	   printf ("Philosopher %d finished eating\n",n);
   }
   return (void*)NULL;
   }

int main()
   {
   int i;
   int checker = 0;
   int deadlock[P_C_COUNT];
   int deadlock_counter = 0;
   srand((unsigned int)time(NULL));

   for(i=0;i<P_C_COUNT;i++)
      pthread_mutex_init(&c[i],NULL);
   for(i=0;i<P_C_COUNT;i++){
      pthread_create(&p[i],NULL,(void *)dine,(void *)(intptr_t)i);// double cast
      deadlock[i] = i;
   }

   while(1){
      for(i=0; i < P_C_COUNT; i++){
          if(deadlockDetector[i] != deadlock[i])
              checker = 1;
      }
      if(!checker)
          deadlock_counter = deadlock_counter + 1;
      else
          deadlock_counter = 0;

      if(deadlock_counter > 4){
        printf("Deadlock detected!\n");
        pthread_kill(p[0],9); // killing 1 thread kills them all
        return 0; // doing 0 since the point of this program is to demonstrate deadlock
      }
      checker = 0;
   }

   return 0;
   }
