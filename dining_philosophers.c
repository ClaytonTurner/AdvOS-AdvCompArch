// compile with: gcc dining_philosophers.c -o dining_philosophers -lpthread


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define WAIT_TIME	1
#define P_C_COUNT	5

pthread_t p[P_C_COUNT];
pthread_mutex_t c[P_C_COUNT];
int deadlockDetector[P_C_COUNT];

void *dine(void *thread)
   {
   int n = (intptr_t) thread;
   int second_chopstick = (n+1)%P_C_COUNT;
   printf("Philosopher %d is grabbing a chopstick\n",n);
   pthread_mutex_lock(&c[n]);
   deadlockDetector[n] = n; // philosopher grabs chopstick of same num initially
   sleep(WAIT_TIME);
   printf("Philosopher %d is attempting to grab a second chopstick\n",n); 
   pthread_mutex_lock(&c[second_chopstick]);
   deadlockDetector[second_chopstick] = n;
   printf("Philosopher %d is eating\n",n);
   sleep(WAIT_TIME);
   pthread_mutex_unlock(&c[n]);
   deadlockDetector[n] = -1;
   pthread_mutex_unlock(&c[second_chopstick]);
   deadlockDetector[second_chopstick] = -1;

   printf ("Philosopher %d finished eating\n",n);

   return (void*)NULL;
   }

int main()
   {
   int i;
   int checker = 0;
   for(i=0;i<P_C_COUNT;i++)
      pthread_mutex_init(&c[i],NULL);

   for(i=0;i<P_C_COUNT;i++)
      pthread_create(&p[i],NULL,(void *)dine,(void *)(intptr_t)i);// double cast

   sleep(5); // make sure initializations are done before we check for deadlock
   while(1){
      for(i=0; i < P_C_COUNT; i++){
          if(deadlockDetector[i] == -1)
              checker = 1;
      }
      if(!checker){
        printf("Deadlock detected!\n");
        for(i=0; i < P_C_COUNT; i++)
           pthread_cancel(p[i]);
        return 0; // doing 0 since the point of this program is to demonstrate deadlock
      }
      checker = 0;
   }

   return 0;
   }
