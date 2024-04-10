#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>

enum flags { b, e, v, n, s, t };

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int flags_array[6] = {0};
    parse(argc, argv, flags_array);
    FILE *fp = NULL;
    int files_count = optind;
    while (files_count != argc) {
      if ((fp = fopen(argv[files_count], "r")) == NULL) {
        fprintf(stderr, "No such file\n");
      } else {
        cat(fp, flags_array);
        fclose(fp);
      }
      files_count++;
    }
  } else {
    fprintf(stderr, "Error\n");
  }
  return 0;
}

void parse(int argc, char *argv[], int *flags_array) {
  opterr = 0;
  const char *short_options = "bevnstET";

  const struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                        {"number", 0, NULL, 'n'},
                                        {"squeeze-blank", 0, NULL, 's'},
                                        {NULL, 0, NULL, 0}};

  int res;

  while ((res = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (res) {
      case 'b':
        flags_array[b] = 1;
        break;
      case 'e':
        flags_array[e] = 1;
        flags_array[v] = 1;
        break;
      case 'v':
        flags_array[v] = 1;
        break;
      case 'n':
        flags_array[n] = 1;
        break;
      case 's':
        flags_array[s] = 1;
        break;
      case 't':
        flags_array[t] = 1;
        flags_array[v] = 1;
        break;
      case 'E':
        flags_array[e] = 1;
        break;
      case 'T':
        flags_array[t] = 1;
        break;
      case '?':
        fprintf(stderr, "Error flag\n");
        break;
    }
  }
  if (flags_array[b] && flags_array[n]) flags_array[n] = 0;
}

void cat(FILE *fp, int *flags_array) {
  int ch_now = '\0';
  int ch_old = '\n';
  int ch_old_old = '\n';
  int count = 1;
  int need_print = 1;
  while ((ch_now = getc(fp)) != EOF) {
    need_print = 1;
    if (flags_array[s]) {
      if (ch_old_old == '\n' && ch_old == '\n' && ch_now == '\n')
        need_print = 0;
    }
    if (flags_array[b] && need_print) {
      if (ch_old == '\n' && ch_now != '\n') {
        printf("%6d\t", count);
        count++;
      }
    }
    if (flags_array[n] && need_print) {
      if (ch_old == '\n') {
        printf("%6d\t", count);
        count++;
      }
    }
    if (flags_array[t]) {
      if (ch_now == 9) {
        putchar('^');
        putchar('I');
        need_print = 0;
      }
    }
    if (flags_array[v]) {
      if (ch_now != 9 && ch_now != 10) {
        if (ch_now >= 0 && ch_now <= 31) {
          printf("^%c", 64 + ch_now);
          need_print = 0;
        } else if (ch_now == 127) {
          printf("^?");
          need_print = 0;
        } else if (ch_now == 255) {
          printf("M-^?");
          need_print = 0;
        } else if (ch_now >= 128 && ch_now <= 159) {
          printf("M-^%c", 64 + ch_now - 128);
          need_print = 0;
        } else if (ch_now >= 160 && ch_now <= 254) {
          printf("M-%c", 32 + ch_now - 160);
          need_print = 0;
        }
      }
    }
    if (flags_array[e] && need_print) {
      if (ch_now == '\n') putchar('$');
    }
    if (need_print) putchar(ch_now);
    ch_old_old = ch_old;
    ch_old = ch_now;
  }
}