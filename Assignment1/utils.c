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
	return n;
}

int strlen(char* str){
  int n = 0;
  while(str != '\0')
    n++;
  return n;
}

int myread(char* file_name, v_struct** p_vec_array_ptr){
	int vectorsLineCount = fileLineCount(file_name);
	int charCount = fileCharCount(file_name);
  //*p_vec_array_ptr = (v_struct*)malloc(vectorsLineCount*sizeof(v_struct));
  v_struct *p_vec_array = (v_struct*)malloc(vectorsLineCount*sizeof(v_struct));
  int i;
	char c;
	char line[charCount];//can assume line is not longer than 100 chars
	int lineIndex = 0;
	int boolean = 1;
	char* temp;
	double temp_r;
	double temp_theta;
	int segfault_runoff = 1;
  double x_comp;
  double y_comp;
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
		if(c == ','){
			line[lineIndex] = '\0';
			temp_r = atof(temp);
			temp = &line[lineIndex+1];
		}
		if(c == '\n'){
			line[lineIndex] = '\0';
			temp_theta = atof(temp);
      if(temp_theta > 360)
        temp_theta = temp_theta - 360; // dont have to worry about other cases
			printf("%.2f,%.2f\n",temp_r,temp_theta);
			p_vec_array->r = temp_r;
			p_vec_array->theta = temp_theta;
      temp_theta = p_vec_array->theta*PI/180;
      x_comp = x_component(p_vec_array);
      y_comp = y_component(p_vec_array);
      printf("r = %.2f theta = %.2f degrees theta = %.2f radians x_comp = %.2f y_comp = %.2f\n",p_vec_array->r, p_vec_array->theta,temp_theta,x_comp,y_comp);
			boolean = 1;
			temp = NULL;
			lineIndex = 0;
			segfault_runoff++;
			if(segfault_runoff < vectorsLineCount)
				p_vec_array_ptr++;
		}
		lineIndex++;
	}
	free(p_vec_array);
	// close the file
	close(fd);
	return 0;
}

double x_component(v_struct* p_vector){
  double x = p_vector->r*cos(p_vector->theta);
	return x;
}

double y_component(v_struct* p_vector){
  double y = p_vector->r*sin(p_vector->theta);
	return y;
}
