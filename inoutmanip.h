#ifndef SO_CPP_INOUTMANIP_H
#define SO_CPP_INOUTMANIP_H

#include "utils.h"

typedef struct input_dir {
	char *dirname;
	struct input_dir *next;
} *InputDirs, **InputD;

int addInputDir(InputD arr, char *dirname, int head_of_list);

int createFileStruct(InputD dir, char *dirname);

int findFile(InputDirs input_dirs, char **input_file);

void freeInputDirs(InputD dir);

void printInputDirs(InputDirs dirs);



#endif
