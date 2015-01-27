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

// Now you put your implementation of the function prototypes here...

/*
 Note the directions/steps in utils.h for each function.  Also, a
 remainder, you can only use the open(), write(), read(), and close()
 I/O system calls.
*/
