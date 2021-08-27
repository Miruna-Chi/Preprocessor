#include "stringmanip.h"
#include "utils.h"

void mystrtok(char *string, int *pos_start, int *pos_stop, int *offset,
	      char *delimiters)
{

	int i, j, token_start, token_end, str_len, del_len, quotes;
	int found_delim_atpos;

	quotes = CLOSED;
	token_start = NOT_FOUND;
	token_end = NOT_FOUND;
	found_delim_atpos = NOT_FOUND;

	str_len = strlen(string);
	del_len = strlen(delimiters);

	for (i = *offset; i < str_len; i++) {
		found_delim_atpos = NOT_FOUND;
		if (string[i] == '\"') {
			if (quotes == CLOSED)
				quotes = OPEN;
			else
				quotes = CLOSED;
		} else if (quotes == CLOSED) {
			for (j = 0; j < del_len; j++)
				if (string[i] == delimiters[j]) {
					if (token_start == FOUND &&
					    token_end == NOT_FOUND) {

						*pos_stop = i - 1;
						token_end = FOUND;
					}
					found_delim_atpos = FOUND;
					break;
				}
			if (found_delim_atpos == NOT_FOUND
			    && token_start == NOT_FOUND) {
				token_start = FOUND;
				*pos_start = i;
			}

			if (token_end == FOUND)
				break;
		}
	}

	if (token_start == FOUND && token_end == NOT_FOUND) {
		/* end of string and token */
		*offset = -1;
		*pos_stop = str_len - 1;
	} else if (token_start == NOT_FOUND) {
		/* end of string and no token */
		*pos_start = -1;
		*offset = -1;
	} else if (token_start == FOUND && token_end == FOUND) {
		*offset = *pos_stop + 2;
	}
}

int replaceInBuf(char *buffer, int start_pos, int end_pos,
		 int length_difference, char *replacement)
{

	int i;
	int str_len;

	str_len = strlen(buffer);

	if (length_difference < 0) {

		for (i = str_len - length_difference + 1; i > end_pos
			- length_difference; i--) {

			buffer[i] = buffer[i + length_difference];
		}
	} else if (length_difference > 0) {
		for (i = end_pos - length_difference + 1;
		     i < str_len - length_difference + 1; i++) {

			buffer[i] = buffer[i + length_difference];
		}
	}

	for (i = start_pos; i <= end_pos - length_difference; i++)
		buffer[i] = replacement[i - start_pos];

	return 0;
}
