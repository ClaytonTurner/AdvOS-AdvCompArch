/* This program is meant to compute the riemann sum via trapezoidal rule
	of a function using a specified step value.
   Threads are synchronized to utilize the main pthread methods
   sin(x) is used as the example function
   2 pi is used as the end of the stepping
*/

#include <stdio.h> 
#include <stdlib.h> /* For ascii to x functions */
#include <pthread.h> /* POSIX threads */
#include <math.h> /* For sin() */
#include "synchronized_traprule.h" /* My header, so quotes */
#include <unistd.h> /* timer */

float h = 0.01; /* Stride size */
static const float h_stride = 0.01;
float x = 0.00; /* Offset for height determination and striding */
float total_area = 0.00; /* Accumulator that will need to be synchronized */
int thread_count; /* Will be given the amount of threads from command line */
int threads_batched = 0;
unsigned int usecs = 100;
int threads_done = 0;

pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER; /* Establishing thread id's */
pthread_mutex_t area_mutex = PTHREAD_MUTEX_INITIALIZER; /* Mutex creation */
pthread_cond_t batch_cond = PTHREAD_COND_INITIALIZER; /* Condition Variable Init */
pthread_mutex_t batch_mutex = PTHREAD_MUTEX_INITIALIZER; /* Required for cond var */
pthread_mutex_t thread_end_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Controller Thread */
void *controller(){
	int i;
	for(;;){
		usleep(usecs); // Avoids rare race condition by giving easier pre-empts
		if(threads_batched >= (thread_count)){
			threads_batched = 0;
			for(i=0;i<thread_count;i++)
				h = h + h_stride;
			//printf("h: %G\n",h);
			//printf("Total Area: %G\n",total_area);
			if(pthread_cond_broadcast(&batch_cond))
				printf("Error\n");
		}
		if(threads_done == thread_count){
			printf("Calculation Done!\n");
			pthread_cond_broadcast(&batch_cond);
			printf("Total area: %G. Expected Area: 4.0\n",total_area);
			exit(0);
			//return NULL;
		}
	}
}

/* thread_index used for modding for riemann area assignment */
void *auc_slice(void *ptr){
	int *thread_index_ptr;
	thread_index_ptr = (int *) ptr;
	float area_to_add;
	int thread_id = *thread_index_ptr;
	pthread_mutex_unlock(&init_mutex); // Unlock from main so we can init thread
	float my_offset = 0.00;
	printf("Thread %d entered\n",thread_id);
	while(my_offset <= 2.0*M_PI){
		/* let's find which h via offset to traverse up and find the area of */
		my_offset = h + thread_id*h_stride;
		// MIGHT NEED TO FIX THIS LOGIC SINCE I'VE CHANGED VARIABLES
		area_to_add = trap_area(h_stride,fabsf(sin(my_offset)),fabsf(sin(my_offset+h_stride)));
		/* Incrementing the total area requires synchronization */
		pthread_mutex_lock(&area_mutex);
		total_area = total_area+area_to_add;
		threads_batched++; /* Let's let the controller know we're done */
		pthread_mutex_unlock(&area_mutex);
		/* condition variable for synchronization
		We want to jump in batches of thread_count 
			in order to exercise posix methods
		*/
		pthread_cond_wait(&batch_cond,&batch_mutex);
	}
	pthread_mutex_lock(&thread_end_mutex);
	threads_done++;
	pthread_mutex_unlock(&thread_end_mutex);
	printf("Thread %d done\n",thread_id);
	while(threads_done != thread_count){
		//printf("Threads done: %d\n",threads_done);
		threads_batched++;
		pthread_cond_wait(&batch_cond,&batch_mutex);
	}
	//printf("threads_batched: %d | thread_count: %d\n",threads_batched,thread_count);
	if(pthread_cond_broadcast(&batch_cond))
		printf("Error\n"); // So the last thread wakes everyone up
	return(NULL);
}

/* length = h; width is the rectangle height; triangle_height is triangle_height.. */
float trap_area(float length,float width,float triangle_height){
	triangle_height = fabsf(triangle_height-width);
	return (length*width)+(length*triangle_height/2.0);
}

struct thread_parameters{
	int thread_id;
	float h;
};

/* Check for formatting
   Create threads
   Threads do heavy-lifting
   Print out results
*/
int main(int argc, char *argv[]){
	/* arg[] = [<filename>,thread_count */
	if (argc != 2 && argc != 3 ) {
		printf("Usage: %s thread_count ?(h)\n",argv[0]);
		return 1;
	}
	if (argc == 2){
		printf("Assuming an h value of 0.01\n");
	}
	else{
		h = atof(argv[2]);
		x = atof(argv[2]);
	}
	//x = h;
	/* Thread creation */
	
	/* Let's create an omnicient control thread */
	pthread_t control_thread;
	//pthread_create(&control_thread,NULL,&controller,NULL);
	pthread_create(&control_thread,NULL,&controller,NULL);
	pthread_t threads[thread_count];
	/* Now let's create the threads for the riemann sum */
	struct thread_parameters t_params;
	t_params.h = h;
	thread_count = atoi(argv[1]);
	int i;
	
	for(i=0;i<thread_count;i++){
		pthread_mutex_lock(&init_mutex); // Locks until thread is ready
		t_params.thread_id = i;
		pthread_create(&threads[i],NULL,&auc_slice,(void *) &t_params);
		
	}
	for(i=0;i<thread_count;i++)
		pthread_join(threads[i],NULL);
	pthread_join(control_thread,NULL);
	/* Expected area determined via: 
	Integral of sin(x) from (0 to PI) * 2  
	This is because we want to do absolute value as per the assignment */
	printf("Approximated area: %G | Expected Area: 4.0\n",total_area);
	return 0;
}
