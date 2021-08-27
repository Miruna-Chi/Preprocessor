#include <stdio.h>
#include <string.h>
#include "utils.h"

int main(int argc, char *argv[])
{
	int i, j, r;
	char *symbol, *mapping, *token;
	int std_out, std_in;
	char *output_file, *input_file;
	char buffer[BUF_SIZE];
	InputDirs input_dirs;
	Hashmap map;

	input_file = NULL;
	output_file = NULL;
	input_dirs = NULL;
	std_in = 1; /* true if there won't be an input file given in argv */
	std_out = 1; /* true if there won't be an output file given in argv */

	r = createHashmap(&map, INITIAL_CAPACITY);
	if (r != 0)
		return r;

	i = 1;
	while (i < argc) {
		/* we encounter one of the options: -D, -I, -o */
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'D') {
				/* if the option is separate from the symbol */
				if (strlen(argv[i]) == 2) {
					/* if there is no argument for
					 * the option, args is invalid
					 */
					if (i == argc - 1)
						return BAD_PARAM;

					token = strtok(argv[i + 1], "=");
					symbol = token;

					token = strtok(NULL, "");
					if (token != NULL)
						mapping = token;
					else
						mapping = NULL;

					r = addElement(map, symbol, mapping);
					if (r != 0)
						return r;
					i += 2;
				} else {
					token = argv[i] + 2;
					token = strtok(token, "=");

					symbol = token;

					token = strtok(NULL, "");
					if (token != NULL)
						mapping = token;
					else
						mapping = NULL;

					r = addElement(map, symbol, mapping);
					if (r != 0)
						return r;
					i++;
				}

			} else if (argv[i][1] == 'I') {
				if (strlen(argv[i]) == 2) {
					/* if there is no argument for
					 * the option, args is invalid
					 */
					if (i == argc - 1)
						return BAD_PARAM;

					r = addInputDir(&input_dirs,
						argv[i + 1], FALSE);
					if (r != 0)
						return r;
					i += 2;
				} else {
					r = addInputDir(&input_dirs,
						argv[i] + 2, FALSE);
					if (r != 0)
						return r;
					i++;
				}

			} else if (argv[i][1] == 'o') {
				if (std_in == 0 && std_out == 0)
					return TOO_MANY_FILES;
				std_out = 0;

				if (strlen(argv[i]) == 2) {
					/* if there is no argument for
					 * the option, args is invalid
					 */
					if (i == argc - 1)
						return BAD_PARAM;

					output_file = calloc(
						strlen(argv[i + 1]) + 1,
						sizeof(char));
					if (output_file == NULL)
						return -ENOMEM;
					strcpy(output_file, argv[i + 1]);
					i += 2;
				} else {
					output_file =
						calloc(strlen(argv[i]) - 1,
						sizeof(char));
					if (output_file == NULL)
						return -ENOMEM;
					strcpy(output_file, argv[i] + 2);

					i++;
				}
			} else {
				return BAD_PARAM;
			}
		} else { /* input file */

			/* if there is no argument for
			 * the option, args is invalid
			 */
			if (std_in == 0 && std_out == 0)
				return TOO_MANY_FILES;
			if (std_in == 0) {
				/* output file found */
				std_out = 0;
				output_file = calloc(strlen(argv[i]) + 1,
						sizeof(char));
				if (output_file == NULL)
					return -ENOMEM;
				strcpy(output_file, argv[i]);
				i++;
				continue;
			}

			std_in = 0;

			input_file = calloc(strlen(argv[i]) + 1, sizeof(char));
			if (input_file == NULL)
				return -ENOMEM;
			strcpy(input_file, argv[i]);

			for (j = strlen(input_file) - 1; j >= 0; j--) {
				if (input_file[j] == '/'
				|| input_file[j] == '\\')
					break;
			}

			if (j > 0) {
				strncpy(buffer, input_file, j);
				buffer[j] = 0;
				r = addInputDir(&input_dirs, buffer, TRUE);
				if (r != 0)
					return r;
			}

			i++;
		}
	}

	r = redirectIOandProcessInput(map, input_dirs, std_in, std_out,
		&input_file, output_file, FALSE);

	if (r != 0)
		return r;

	freeDataStructures(&map, &input_dirs, &input_file, &output_file);

	return r;
}
