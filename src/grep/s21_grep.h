#ifndef S21_GREP_H
#define S21_GREP_H

#include <stdio.h>

char *parse(int argc, char *argv[], int *flags_array, char *pattern,
            int *files_ind);
char *getPattern(char *pattern, char *line, size_t line_len,
                 int *pattern_temp_ind);
void grep(char *argv[], FILE *fp, char *pattern, int *flags_array,
          int *files_ind, int one_file);

#endif  // S21_GREP_H
