/* -----------------------------------
 CSIS 614 - Advanced Operating Systems
 Spring 2015 (Tony Leclerc)
 utils.c implementation file
 Assignment 1
 -----------------------------------
*/

/* ----------------------------------------------
 These are the ONLY system libraries that can be used.
 Under no circumstances can additional libraries be included
*/

#include <unistd.h>	// for 'basic' I/O functions
#include <fcntl.h>
#include <stdlib.h> 	// atof() function, malloc()
#include <math.h>	// math functions (eg. sin(), cos())

#include "utils.h"	// your own functions
#include <stdio.h>	//debugging
#include <errno.h>	//debugging
// Now you put your implementation of the function prototypes here...

/*
 Note the directions/steps in utils.h for each function.  Also, a
 remainder, you can only use the open(), write(), read(), and close()
 I/O system calls.
*/
int fileCharCount(char* file_name){
	int n = 0; // counts number of chars
	char c;
	int filedesc = open(file_name, O_RDONLY);

	if(filedesc < 0) {
		write(2,"Error opening file\n",19);
		return -1; 
	}
	while(read(filedesc,&c,1) > 0){
		n++;
	}
	close(filedesc);
	return n;
}


int fileLineCount(char* file_name){
	int n = 0; // counts number of lines
	char c; // checks for new lines
	int filedesc = open(file_name, O_RDONLY);

	if(filedesc < 0) {
		write(2,"Error opening file\n",19);
		return -1; 
	}
	while(read(filedesc,&c,1) > 0){
		if(c == '\n')
			n++;// set up so no off-by-1 and no extra incrementing needed
	}
	close(filedesc);
	/*if(closeFile<0){
		write(2,"Error closing file\n",19);
		return -1;
	}*/	
	return n;
}

int myread(char* file_name, v_struct** p_vec_array_ptr){
	int vectorsLineCount = fileLineCount(file_name);
	int charCount = fileCharCount(file_name);
	v_struct* p_vec_array = (v_struct*)malloc(vectorsLineCount*sizeof(v_struct));
	int i;
	char c;
	char line[charCount];//can assume line is not longer than 100 chars
	int lineIndex;
	int boolean = 1;
	char* temp;
	double temp_r;
	double temp_theta;
	int segfault_runoff = 1;
	v_struct* p_vec_array_original = p_vec_array;
	// open the file
	int fd = open(file_name, O_RDONLY);
	if(fd < 0){
		write(2,"Error opening file\n",19);
		return -1;
	}	
	
	for(i = 0; i < charCount; i++){
		read(fd,&c,1);
		line[lineIndex] = c;
		if(boolean == 1){
			boolean = 0;
			temp = &line[lineIndex];
		}
		//printf("%c\n",line[lineIndex]);
		if(c == ','){
			line[lineIndex] = '\0';
			temp_r = atof(temp);
			temp = &line[lineIndex+1];
		}
		if(c == '\n'){
			line[lineIndex] = '\0';
			temp_theta = atof(temp);
			printf("%.2f,%.2f\n",temp_r,temp_theta);
			p_vec_array->r = temp_r;
			p_vec_array->theta = temp_theta;
			boolean = 1;
			temp = NULL;
			lineIndex = 0;
			printf("test: %f\n",p_vec_array->r);
			segfault_runoff++;
			if(segfault_runoff < vectorsLineCount)
				p_vec_array++;
		}
		lineIndex++;
	}
	p_vec_array_ptr = &p_vec_array_original;
	//free(p_vec_array);
	// close the file
	close(fd);
	/*if(closeFile < 0){
		write(2,"Error closing file\n",19);
		printf("Continuing since this is a bug\n");
		//return -1;
	}*/
	return 0;
}

double x_component(v_struct* p_vector){

	return 0.0;
}

double y_component(v_struct* p_vector){

	return 0.0;
}
