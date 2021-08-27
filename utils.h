#ifndef SO_CPP_UTILS_H
#define SO_CPP_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.h"
#include "stringmanip.h"
#include "inoutmanip.h"
#include "preprocessing.h"

/* error codes */
#define ENOMEM (-12)
#define FILE_NOT_FOUND 1
#define BAD_PARAM 2
#define TOO_MANY_FILES 3
#define DELIMITERS (" []{}<>=+-*/%!&|^.,:;()")

#define TRUE 1
#define FALSE 0

#define OPEN 1
#define CLOSED 0

#define NOT_FOUND 0
#define FOUND 1

#define OUT_OF_RANGE (-1)

/* initial map capacity */
#define INITIAL_CAPACITY 4

#define BUF_SIZE 256
#define SIZE 50

void freeDataStructures(HashM map, InputD dirs, char **input_file,
	char **output_file);

#define DIE(condition, msg) \
	do { \
		if (condition) { \
			fprintf(stderr, "(%s:%d): %s\n", __FILE__, \
			__LINE__, msg); \
			perror(""); \
			exit(1); \
		} \
	} while (0)


#endif
