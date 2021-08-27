#include "preprocessing.h"

char buffer[BUF_SIZE];
char buf_cpy[BUF_SIZE];
char output[BUF_SIZE];
struct if_tracking if_track[SIZE];
int pos = -1;

int redirectIOandProcessInput(Hashmap map, InputDirs input_dirs, int std_in,
	int std_out, char **input_file, char *output_file, int is_header)
{

	int r;
	FILE *in, *out;
	int search_for_input_file;
	char *delimiters;

	delimiters = calloc(strlen(DELIMITERS) + 3, sizeof(char));
	if (delimiters == NULL)
		return -ENOMEM;

	delimiters[0] = '\t';
	delimiters[1] = '\\';
	strcat(delimiters, DELIMITERS);

	in = NULL;
	out = NULL;

	if (!std_in) {
		search_for_input_file = findFile(input_dirs, input_file);
		if (search_for_input_file != 0)
			return search_for_input_file;

		in = fopen(*input_file, "r");
	} else
		in = stdin;

	if (!std_out && !is_header)
		out = freopen(output_file, "w", stdout);


	r = processInput(map, input_dirs, in, delimiters);
	if (r != 0)
		return r;

	free(delimiters);

	if (!std_in)
		fclose(in);
	if (!std_out && !is_header)
		fclose(out);
	return 0;
}

int processInput(Hashmap map, InputDirs input_dirs, FILE *in, char *delimiters)
{
	int pos_start, pos_stop, offset;
	char *token, *symbol;
	int r;
	SymbolMapping found_elem;

	while (fgets(buffer, BUF_SIZE, in) != NULL) {

		if (strlen(buffer) == 1) {
			/* it's endline */
			continue;
		}
		buffer[strlen(buffer) - 1] = 0;
		strcpy(buf_cpy, buffer);

		strcpy(output, buffer);
		token = strtok(buffer, delimiters);

		/*
		 * directive == define, init symbol, init mapping, wait for
		 * more possible mapping on the next lines
		 */
		if (pos >= 0 && strcmp(token, "#if") != 0
		    && strcmp(token, "#elif") != 0
		    && strcmp(token, "#else") != 0
		    && strcmp(token, "#endif") != 0
		    && strcmp(token, "#ifdef") != 0
		    && strcmp(token, "#ifndef") != 0) {

			if (if_track[pos].is_else && if_track[pos].if_cond)
				continue;

			if (if_track[pos].is_if && !if_track[pos].if_cond)
				continue;
		}

		if (token != NULL) {
			if (strcmp(token, "#define") == 0) {

				token = strtok(NULL, " ");
				r = processDefine(map, in, delimiters, &token);
				if (r != 0)
					return r;
				continue;
			} else if (strcmp(token, "#undef") == 0) {
				token = strtok(NULL, " ");

				symbol = calloc(strlen(token) + 1,
						sizeof(char));
				if (symbol == NULL)
					return -ENOMEM;
				strcpy(symbol, token);

				token = strtok(NULL, " ");

				removeElement(map, symbol);
				free(symbol);

				continue;

			} else if ((strcmp(token, "#if") == 0
				    || strcmp(token, "#elif") == 0)) {

				int value;

				/*
				 * if increments number of ifs that have an end,
				 * elif doesn't
				 */
				if (token[1] == 'i') {
					pos++;
					if_track[pos].is_if = TRUE;
					if_track[pos].is_else = FALSE;
				}
				token = strtok(NULL, " ");
				value = atoi(token);

				if (value != 0)
					if_track[pos].if_cond = TRUE;
				else {
					found_elem = find(map, token);
					if (found_elem != NULL)
						value = atoi(
							found_elem->mapping);
					if (value != 0)
						if_track[pos].if_cond = TRUE;
					else if_track[pos].if_cond = FALSE;
				}
				continue;
			} else if (strcmp(token, "#endif") == 0) {
				pos--;
				continue;
			} else if (strcmp(token, "#else") == 0) {
				if_track[pos].is_else = TRUE;
				if_track[pos].is_if = FALSE;
				continue;
			} else if (strcmp(token, "#ifdef") == 0) {

				pos++;
				if_track[pos].is_if = TRUE;
				if_track[pos].is_else = FALSE;

				token = strtok(NULL, " ");
				found_elem = find(map, token);

				if (found_elem != NULL)
					if_track[pos].if_cond = TRUE;
				else if_track[pos].if_cond = FALSE;

				continue;
			} else if (strcmp(token, "#ifndef") == 0) {

				pos++;
				if_track[pos].is_if = TRUE;
				if_track[pos].is_else = FALSE;

				token = strtok(NULL, " ");
				found_elem = find(map, token);

				if (found_elem != NULL)
					if_track[pos].if_cond = FALSE;
				else if_track[pos].if_cond = TRUE;

				continue;
			} else if (strcmp(token, "#include") == 0) {
				char *header_file;

				token = strtok(NULL, " \"");

				header_file = calloc(strlen(token) + 1,
						     sizeof(char));
				if (header_file == NULL)
					return -ENOMEM;

				strcpy(header_file, token);

				r = redirectIOandProcessInput(map, input_dirs,
					FALSE, TRUE, &header_file,
					NULL, TRUE);
				if (r != 0)
					return r;

				free(header_file);

				continue;
			}
		}

		pos_start = 0;
		pos_stop = 0;
		offset = 0;

		while (offset != OUT_OF_RANGE) {

			mystrtok(output, &pos_start, &pos_stop,
				 &offset, delimiters);
			if (pos_start == OUT_OF_RANGE)
				break;

			token = calloc(pos_stop - pos_start + 2, sizeof(char));
			if (token == NULL)
				return -ENOMEM;

			strncpy(token, output + pos_start,
				pos_stop - pos_start + 1);

			found_elem = find(map, token);

			if (found_elem != NULL) {
				int length_difference;

				length_difference = strlen(token) -
						    strlen(found_elem->mapping);

				r = replaceInBuf(output, pos_start, pos_stop,
						 length_difference,
						 found_elem->mapping);
				if (r != 0)
					return r;
				offset -= length_difference;
			}
			free(token);
		}

		printf("%s\n", output);
	}
	return 0;
}

int processDefine(Hashmap map, FILE *in, char *delimiters, char **token)
{
	int multiple_line_define, r, map_len;
	char *symbol, *mapping;
	SymbolMapping elem;

	multiple_line_define = 0;

	symbol = calloc(strlen(*token) + 1, sizeof(char));
	if (symbol == NULL)
		return -ENOMEM;
	strcpy(symbol, *token);

	*token = strtok(NULL, " ");
	/* find possibly nested symbols */
	if (*token != NULL) {
		elem = find(map, *token);
		if (elem != NULL)
			*token = elem->mapping;
		mapping = calloc(strlen(*token) + 1, sizeof(char));
		if (mapping == NULL)
			return -ENOMEM;
		strcpy(mapping, *token);

		*token = strtok(NULL, " ");
	} else
		mapping = NULL;
	while (*token != NULL && *token[0] != '\\') {
		map_len = strlen(mapping);
		elem = find(map, *token);
		if (elem != NULL)
			*token = elem->mapping;
		mapping = realloc(mapping, map_len + strlen(*token) + 2);
		if (mapping == NULL)
			return -ENOMEM;

		mapping[map_len] = ' ';
		mapping[map_len + 1] = 0;
		strcat(mapping, *token);
		*token = strtok(NULL, " ");
	}

	/*
	 * if the line ends in a '\', then define hasn't ended
	 * sent that information to the next iteration
	 */
	if (buf_cpy[strlen(buf_cpy) - 1] == '\\')
		multiple_line_define = 1;

	while (multiple_line_define) {
		if (fgets(buffer, BUF_SIZE, in) == NULL)
			break;
		map_len = strlen(mapping);

		if (strlen(buffer) == 1) {
			/* it's endline */
			continue;
		}
		buffer[strlen(buffer) - 1] = 0;
		strcpy(buf_cpy, buffer);

		strcpy(output, buffer);
		*token = strtok(buffer, " ");

		/* find possibly nested symbols */
		elem = find(map, *token);
		if (elem != NULL)
			*token = elem->mapping;
		mapping = realloc(mapping, map_len + strlen(*token) + 2);
		if (mapping == NULL)
			return -ENOMEM;
		mapping[map_len] = ' ';
		mapping[map_len + 1] = 0;
		strcat(mapping, *token);
		*token = strtok(NULL, " \\");

		while (*token != NULL) {
			map_len = strlen(mapping);
			/* find possibly nested symbols */
			elem = find(map, *token);
			if (elem != NULL)
				*token = elem->mapping;
			mapping = realloc(mapping, map_len +
						   strlen(*token) + 2);

			if (mapping == NULL)
				return -ENOMEM;

			mapping[map_len] = ' ';
			mapping[map_len + 1] = 0;
			strcat(mapping, *token);
			*token = strtok(NULL, " \\");
		}

		if (buf_cpy[strlen(buf_cpy) - 1] != '\\')
			multiple_line_define = 0;
	}

	/* define has ended */
	r = addElement(map, symbol, mapping);
	if (r != 0)
		return r;
	free(symbol);
	free(mapping);

	return 0;
}

int processIf(Hashmap map, FILE *in, char *delimiters, char **token)
{
	return 0;
}
