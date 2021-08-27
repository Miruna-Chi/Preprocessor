#include "utils.h"

void freeDataStructures(HashM map, InputD dirs, char **input_file,
	char **output_file)
{

	freeHashmap(map);
	freeInputDirs(dirs);
	if (*input_file != NULL)
		free(*input_file);
	if (*output_file != NULL)
		free(*output_file);
}
