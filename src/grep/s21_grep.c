#define _GNU_SOURCE

#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum flags { e, i, v, c, l, n, h, s, f, o };

int main(int argc, char *argv[]) {
  if (argc > 2) {
    int flags_array[10] = {0};
    char *pattern = NULL;
    int files_ind = 0;
    pattern = parse(argc, argv, flags_array, pattern, &files_ind);
    FILE *fp = NULL;
    int one_file = 0;
    if (files_ind == argc - 1) one_file = 1;
    if (pattern != NULL)
      while (files_ind != argc) {
        if (!(fp = fopen(argv[files_ind], "r"))) {
          if (!flags_array[s]) fprintf(stderr, "Error\n");
          files_ind++;
          continue;
        } else {
          grep(argv, fp, pattern, flags_array, &files_ind, one_file);
          fclose(fp);
        }
      }
    free(pattern);
  } else {
    fprintf(stderr, "Error\n");
  }
  return 0;
}

char *parse(int argc, char *argv[], int *flags_array, char *pattern,
            int *files_ind) {
  int parse_result = 0;
  int pattern_temp_ind = 0;
  FILE *fp = NULL;
  char *line = NULL;
  size_t line_len = 0;
  while ((parse_result = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) !=
         -1) {
    switch (parse_result) {
      case 'e':
        flags_array[e] = 1;
        pattern =
            getPattern(pattern, optarg, strlen(optarg), &pattern_temp_ind);
        break;
      case 'i':
        flags_array[i] = 1;
        break;
      case 'v':
        flags_array[v] = 1;
        break;
      case 'c':
        flags_array[c] = 1;
        break;
      case 'l':
        flags_array[l] = 1;
        break;
      case 'n':
        flags_array[n] = 1;
        break;
      case 'h':
        flags_array[h] = 1;
        break;
      case 's':
        flags_array[s] = 1;
        break;
      case 'f':
        flags_array[f] = 1;
        if (!(fp = fopen(optarg, "r"))) {
          if (!flags_array[s]) fprintf(stderr, "Error\n");
        } else {
          while (getline(&line, &line_len, fp) != -1)
            pattern =
                getPattern(pattern, line, strlen(line), &pattern_temp_ind);
          fclose(fp);
        }
        break;
      case 'o':
        flags_array[o] = 1;
        break;
      case '?':
        fprintf(stderr, "Error\n");
    }
  }
  if (line) free(line);
  *files_ind = optind;
  if (flags_array[e] == 0 && flags_array[f] == 0) {
    pattern = getPattern(pattern, argv[optind], strlen(argv[optind]),
                         &pattern_temp_ind);
    *files_ind += 1;
  }
  pattern[pattern_temp_ind - 1] = '\0';
  if (flags_array[l]) flags_array[c] = 0;
  if ((flags_array[v] && flags_array[o] && !flags_array[c] &&
       !flags_array[l]) ||
      (strstr(pattern, "\\") != NULL)) {
    if (strstr(pattern, "\\") != NULL) fprintf(stderr, "Error\n");
    if (pattern) {
      free(pattern);
      pattern = NULL;
    }
  }
  return pattern;
}

char *getPattern(char *pattern, char *line, size_t line_len,
                 int *pattern_temp_ind) {
  pattern = (char *)realloc(pattern, *pattern_temp_ind + line_len + 2);
  strncpy(pattern + *pattern_temp_ind, line, line_len);
  *pattern_temp_ind += line_len;
  if (pattern[*pattern_temp_ind - 1] == '\n' && line_len > 1)
    pattern = (char *)realloc(pattern, (*pattern_temp_ind)-- + 1);
  pattern[(*pattern_temp_ind)++] = '|';
  pattern[*pattern_temp_ind] = '\0';
  return pattern;
}

void grep(char *argv[], FILE *fp, char *pattern, int *flags_array,
          int *files_ind, int one_file) {
  regex_t regex;
  if (flags_array[i])
    regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE | REG_NEWLINE);
  else
    regcomp(&regex, pattern, REG_EXTENDED | REG_NEWLINE);
  const size_t nmatch = 1;
  regmatch_t pmatch[nmatch];
  int lines_count = 1;
  int c_flag_count = 0;
  int end = 0;
  char *ptr = NULL;
  int result = 0;
  char *line = NULL;
  size_t line_len = 0;
  while (getline(&line, &line_len, fp) != -1) {
    if (flags_array[l]) {
      if (((result = regexec(&regex, line, 0, 0, 0)) && flags_array[v]) ||
          (!result && !flags_array[v])) {
        printf("%s\n", argv[*files_ind]);
        break;
      }
    } else if (flags_array[c]) {
      if (((result = regexec(&regex, line, 0, 0, 0)) && flags_array[v]) ||
          (!result && !flags_array[v]))
        c_flag_count++;
    } else if (flags_array[o]) {
      end = 0;
      ptr = line;
      while (!end) {
        if (((result = regexec(&regex, ptr, nmatch, pmatch, 0)) &&
             flags_array[v]) ||
            (!result && !flags_array[v])) {
          if (!one_file && !flags_array[h]) printf("%s:", argv[*files_ind]);
          if (flags_array[n]) printf("%d:", lines_count);
          for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++)
            putchar(ptr[i]);
          putchar('\n');
          ptr += pmatch[0].rm_eo;
        } else {
          end = 1;
          ptr = NULL;
        }
      }
    } else {
      if (((result = regexec(&regex, line, 0, 0, 0)) && flags_array[v]) ||
          (!result && !flags_array[v])) {
        if (!one_file && !flags_array[h]) printf("%s:", argv[*files_ind]);
        if (flags_array[n]) printf("%d:", lines_count);
        if (strchr(line, '\n'))
          printf("%s", line);
        else
          printf("%s\n", line);
      }
    }
    lines_count++;
  }
  if (flags_array[c]) {
    if (!one_file && !flags_array[h])
      printf("%s:%d\n", argv[*files_ind], c_flag_count);
    else
      printf("%d\n", c_flag_count);
    c_flag_count = 0;
  }
  (*files_ind)++;
  if (line) free(line);
  regfree(&regex);
}
