#ifndef SO_CPP_STRINGMANIP_H
#define SO_CPP_STRINGMANIP_H

void mystrtok(char *string, int *pos_start, int *pos_stop, int *offset,
	      char *delimiters);

int replaceInBuf(char *buffer, int start_pos, int end_pos,
		 int length_difference, char *replacement);
#endif
