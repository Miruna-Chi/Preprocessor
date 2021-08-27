#include "utils.h"

int findFile(InputDirs input_dirs, char **input_file)
{
	InputDirs current_dir;
	FILE *file_exists;
	int search_for_file = 0;

	current_dir = input_dirs;

	file_exists = fopen(*input_file, "r");
	if (file_exists != NULL) {
		fclose(file_exists);
		return 0;
	}

	while (current_dir != NULL && search_for_file == 0) {
		char *file_path = calloc(strlen(current_dir->dirname) +
			strlen(*input_file) + 2, sizeof(char));
		if (file_path == NULL)
			return -ENOMEM;

		strcpy(file_path, current_dir->dirname);
		strcat(file_path, "/");
		strcat(file_path, *input_file);

		file_exists = fopen(file_path, "r");
		if (file_exists == NULL) {
			current_dir = current_dir->next;
			free(file_path);
		} else {
			search_for_file = 1;
			free(*input_file);
			*input_file = file_path;
			fclose(file_exists);
		}
	}

	if (search_for_file == 1)
		return 0;
	return 1;
}

int addInputDir(InputD arr, char *dirname, int head_of_list)
{
	InputDirs current_dir = *arr;
	InputDirs prev_dir;
	int r;

	if (current_dir == NULL) {
		r = createFileStruct(&current_dir, dirname);
		if (r != 0)
			return r;
		*arr = current_dir;

		return 0;
	} else if (head_of_list) {
		r = createFileStruct(&current_dir, dirname);
		if (r != 0)
			return r;
		current_dir->next = *arr;
		*arr = current_dir;
		return 0;
	}

	while (current_dir != NULL) {
		prev_dir = current_dir;
		current_dir = current_dir->next;
	}

	if (prev_dir != NULL) {
		r = createFileStruct(&current_dir, dirname);
		if (r != 0)
			return r;
		prev_dir->next = current_dir;
	}
	return 0;
}

int createFileStruct(InputD dir, char *dirname)
{
	*dir = malloc(sizeof(struct input_dir));
	if (*dir == NULL)
		return -ENOMEM;
	(*dir)->dirname = calloc(strlen(dirname) + 1, sizeof(char));
	if ((*dir)->dirname == NULL)
		return -ENOMEM;
	strcpy((*dir)->dirname, dirname);
	(*dir)->next = NULL;

	return 0;
}

void freeInputDirs(InputD dir)
{
	InputDirs current_dir = *dir;
	InputDirs prev;

	while (current_dir != NULL) {
		prev = current_dir;
		current_dir = current_dir->next;
		free(prev->dirname);
		free(prev);
	}
}

void printInputDirs(InputDirs dirs)
{
	InputDirs current_indir;

	current_indir = dirs;
	printf("\nInputDirs:\n");
	while (current_indir != NULL) {
		printf("%s\n", current_indir->dirname);
		current_indir = current_indir->next;
	}
}
