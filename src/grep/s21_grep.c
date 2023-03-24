#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 2048

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char pattern[MAX_LENGTH];
  int regular;
  int number_of_files;
} flags;

int search_flags(int argc, char *argv[], flags *flag);
void grep_files(char *argv[], flags *flag);
void print_result(char *argv[], flags *flag, int number_of_string,
                  char *temp_line);
void print_c(char *argv[], flags *flag, int find_success, int number_of_string);
void check_l(char *argv[], int *flag_l_exit);
int regularFlagF(char *optarg, flags *flag);

int main(int argc, char *argv[]) {
  if (argc > 2) {
    opterr = 0;
    flags fl = {0};
    flags *flag = &fl;
    if (search_flags(argc, argv, flag) == 0) {
      if (fl.regular == 0) {
        strcat(fl.pattern, argv[optind]);
        optind++;
      }
      if (argc - optind > 1) {
        flag->number_of_files = 1;
      }
      while (optind < argc) {
        grep_files(argv, flag);
        optind++;
      }
    }
  }
  return 0;
}
int search_flags(int argc, char *argv[], flags *flag) {
  int res = 0, flagOut = 0;
  char *ch = "|";
  while ((res = getopt_long(argc, argv, "e:ivclnhsf:o", 0, 0)) != -1) {
    switch (res) {
      case 'e':
        flag->e = 1;
        if ((strlen(flag->pattern) + strlen(optarg) < MAX_LENGTH - 2)) {
          if (strlen(optarg) != 0) {
            if (flag->regular != 0) {
              strcat(flag->pattern, ch);
            }
            strcat(flag->pattern, optarg);
            flag->regular = 1;
          } else {
            fprintf(stderr, "grep: option requires an argument -- 'e'\n");
            flagOut = 1;
            break;
          }
        }
        break;
      case 'i':
        flag->i = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'c':
        flag->c = 1;
        break;
      case 'l':
        flag->l = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 'h':
        flag->h = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'f':
        flag->f = 1;
        if (regularFlagF(optarg, flag) == 1) {
          flagOut = 1;
        }
        break;
      case 'o':
        flag->o = 1;
        break;
      default:
        fprintf(stderr, "grep:  invalid option\n");
        flagOut = 1;
        break;
    }
    if (flag->c == 1) flag->n = 0;
  }
  return flagOut;
}
void grep_files(char *argv[], flags *flag) {
  FILE *f = NULL;
  f = fopen(argv[optind], "r");
  if (f != NULL) {
    regex_t compiled;
    int cflags = REG_EXTENDED, flag_l_exit = 0, number_of_string = 0,
        success = 0, find_success = 0;
    long int reedChars = 0;
    if (flag->i) cflags = REG_EXTENDED | REG_ICASE;
    char *temp_line = NULL;
    size_t length;
    if (regcomp(&compiled, flag->pattern, cflags) == 0) {
      while ((reedChars = getline(&temp_line, &length, f)) != -1) {
        number_of_string++;
        if (temp_line[strlen(temp_line) - 1] == '\n') {
          temp_line[strlen(temp_line) - 1] = '\0';
        }
        success = regexec(&compiled, temp_line, 0, NULL, 0);
        if (success == 0) {
          if (flag->c == 1) {
            find_success++;
            continue;
          }
          if (flag->l == 1 && flag->v == 0) {
            check_l(argv, &flag_l_exit);
            continue;
          }
          if (flag->l == 0 && flag->v == 0) {
            print_result(argv, flag, number_of_string, temp_line);
          }
        }
        if (success != 0 && flag->v == 1) {
          if (flag->l == 1 && flag->c == 0) {
            check_l(argv, &flag_l_exit);
            continue;
          }
          if (flag->n == 0 && flag->c == 0) {
            if (flag->number_of_files == 1 && flag->h == 0)
              printf("%s:", argv[optind]);
            printf("%s\n", temp_line);
          }
          if (flag->n == 1 && flag->c == 0) {
            if (flag->number_of_files == 1 && flag->h == 0)
              printf("%s:", argv[optind]);
            printf("%d:%s\n", number_of_string, temp_line);
          }
        }
      }
      free(temp_line);
      if (flag->c) print_c(argv, flag, find_success, number_of_string);
      regfree(&compiled);
    } else {
      fprintf(stderr, "grep: regcomp ERROR\n");
    }
    fclose(f);
  } else {
    if (flag->s == 0)
      fprintf(stderr, "grep: %s : No such file or directory\n", argv[optind]);
  }
}
void print_result(char *argv[], flags *flag, int number_of_string,
                  char *temp_line) {
  if (flag->h == 0) {
    if (flag->n == 0) {
      if (flag->number_of_files == 0) printf("%s\n", temp_line);
      if (flag->number_of_files == 1)
        printf("%s:%s\n", argv[optind], temp_line);
    }
    if (flag->n == 1) {
      if (flag->number_of_files == 0) {
        printf("%d:%s\n", number_of_string, temp_line);
      }
      if (flag->number_of_files == 1) {
        printf("%s:%d:%s\n", argv[optind], number_of_string, temp_line);
      }
    }
  }
  if (flag->h == 1) {
    if (flag->n == 0) printf("%s\n", temp_line);
    if (flag->n == 1) printf("%d:%s\n", number_of_string, temp_line);
  }
}
void print_c(char *argv[], flags *flag, int find_success,
             int number_of_string) {
  if (flag->v == 1 && flag->l == 1) {
    if (flag->number_of_files == 1 && flag->h == 0) printf("%s:", argv[optind]);
    printf("1\n%s\n", argv[optind]);
  }
  if (flag->v == 1 && flag->l == 0) {
    if (flag->number_of_files == 1 && flag->h == 0) printf("%s:", argv[optind]);
    printf("%d\n", number_of_string - find_success);
  }
  if (flag->v == 0 && flag->l == 0) {
    if (flag->number_of_files == 1 && flag->h == 0) printf("%s:", argv[optind]);
    printf("%d\n", find_success);
  }
  if (flag->v == 0 && flag->l == 1) {
    if (flag->number_of_files == 1 && flag->h == 0) printf("%s:", argv[optind]);
    if (find_success > 0) printf("1\n%s\n", argv[optind]);
    if (find_success == 0) printf("0\n");
  }
}
void check_l(char *argv[], int *flag_l_exit) {
  if (*flag_l_exit != optind) {
    printf("%s\n", argv[optind]);
    *flag_l_exit = optind;
  }
}
int regularFlagF(char *optarg, flags *flag) {
  FILE *reg = NULL;
  int err = 0;
  if ((reg = fopen(optarg, "r")) != NULL) {
    long int reedChars = 0;
    char *temp_line = NULL;
    size_t length;
    char *ch = "|";
    while ((reedChars = getline(&temp_line, &length, reg)) != -1) {
      if (strlen(temp_line) != 0) {
        if (flag->regular != 0) {
          strcat(flag->pattern, ch);
        }
        if (strlen(temp_line) > 1 && temp_line[strlen(temp_line) - 1] == '\n') {
          temp_line[strlen(temp_line) - 1] = '\0';
        }
        strcat(flag->pattern, temp_line);
        flag->regular = 1;
      }
    }
    free(temp_line);
    fclose(reg);
  } else {
    if (flag->s == 0) {
      fprintf(stderr, "grep: %s : No such file or directory\n", optarg);
      err = 1;
    }
  }
  return err;
}
