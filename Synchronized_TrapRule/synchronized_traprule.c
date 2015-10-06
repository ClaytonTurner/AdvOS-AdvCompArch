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

float h = 0.01;
float x;
float total_area = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* thread_index used for modding for riemann area assignment */
void *auc_slice(void *ptr){
	int *thread_index_ptr;
	thread_index_ptr = (int *) ptr;
	float area_to_add;
	printf("Thread %d entered\n",*thread_index_ptr);
	
	//while(x <= 2*M_PI)
	area_to_add = trap_area(h,fabsf(sin(x+) ),)
	
	pthread_mutex_lock(&mutex);
	total_area = total_area+area_to_add;
	printf("Total area: %G\n",total_area);
	pthread_mutex_unlock(&mutex);
}

/* length = h; width is the rectangle height; triangle_height is triangle_height.. */
float trap_area(int length,int width,int triangle_height){
	return ((float)length*(float)width)+((float)length*(float)triangle_height/2.0);
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
	}
	x = h;
	/* Thread creation */
	struct thread_parameters t_params;
	t_params.h = h;
	int thread_count = atoi(argv[1]);
	int i;
	pthread_t threads[thread_count];
	for(i=0;i<thread_count;i++){
		t_params.thread_id = i;
		pthread_create(&threads[i],NULL,&auc_slice,(void *) &t_params);
		pthread_join(threads[i],NULL);
	}
	return 0;
}
