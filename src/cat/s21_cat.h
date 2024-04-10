#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>

void parse(int argc, char *argv[], int *flags_array);
void cat(FILE *fp, int *flags_array);

#endif  // S21_CAT_H