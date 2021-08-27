#ifndef SO_CPP_PREPROCESSING_H
#define SO_CPP_PREPROCESSING_H

#include "utils.h"

struct if_tracking {
	int if_cond;
	int is_if;
	int is_else;
};

int redirectIOandProcessInput(Hashmap map, InputDirs input_dirs, int std_in,
	int std_out, char **input_file, char *output_file, int is_header);

int processInput(Hashmap map, InputDirs input_dirs, FILE *in,
	char *delimiters);

int processDefine(Hashmap map, FILE *in, char *delimiters, char **token);

int processIf(Hashmap map, FILE *in, char *delimiters, char **token);

#endif
