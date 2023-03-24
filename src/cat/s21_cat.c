#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
} flags;

void set_data(flags *flag);
int set_flags(int argc, char *argv[], flags *flag, int *p_counter);
void use_flags(char *str, flags *flag);
void check_flag(char now, flags *flag, char *prev);

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int counter = 1;
    int *p_counter = &counter;
    flags fl;
    flags *flag = &fl;
    set_data(flag);
    if (set_flags(argc, argv, flag, p_counter) == 0) {
      for (int i = counter; i < argc; i++) {
        if ((argv[i][0]) != '-') {
          use_flags(argv[i], flag);
        } else {
          fprintf(stderr, "cat: %s : No such file or directory\n", argv[i]);
          continue;
        }
      }
    }
  }
  return 0;
}
void set_data(flags *flag) {
  flag->b = 0;
  flag->e = 0;
  flag->n = 0;
  flag->s = 0;
  flag->t = 0;
  flag->v = 0;
  flag->T = 0;
  flag->E = 0;
}
int set_flags(int argc, char *argv[], flags *flag, int *p_counter) {
  int err = 0;
  for (int i = 1; i < argc; i++, (*p_counter)++) {
    if ((argv[i][0]) != '-') {
      break;
    }
    if ((strcmp(argv[i], "-b") == 0) ||
        (strcmp(argv[i], "--number-nonblank") == 0)) {
      flag->b = 1;
    } else if (strcmp(argv[i], "-e") == 0) {
      flag->e = 1;
    } else if (strcmp(argv[i], "-E") == 0) {
      flag->E = 1;
    } else if ((strcmp(argv[i], "-n") == 0) ||
               (strcmp(argv[i], "--number") == 0)) {
      flag->n = 1;
    } else if ((strcmp(argv[i], "-s") == 0) ||
               (strcmp(argv[i], "--squeeze-blank") == 0)) {
      flag->s = 1;
    } else if (strcmp(argv[i], "-t") == 0) {
      flag->t = 1;
    } else if (strcmp(argv[i], "-T") == 0) {
      flag->T = 1;
    } else if ((strcmp(argv[i], "-v") == 0) ||
               (strcmp(argv[i], "--show-nonprinting") == 0)) {
      flag->v = 1;
    } else {
      fprintf(stderr,
              "cat: illegal option -%s \nusage: cat [-benstuv] [file ...]\n",
              argv[i]);
      err = 1;
    }
  }
  return err;
}
void use_flags(char *str, flags *flag) {
  FILE *f = fopen(str, "r");
  if (NULL == f) {
    fprintf(stderr, "cat: %s : No such file or directory\n", str);
  } else {
    char now = ' ', prev = '\n';
    int counter_lines = 1;
    if ((flag->b + flag->e + flag->n + flag->s + flag->t + flag->v + flag->T +
         flag->E) != 0) {
      while ((now = fgetc(f)) != EOF) {
        if (flag->s == 1 && prev == '\n' && now == '\n') {
          char next = fgetc(f);
          ungetc(next, f);
          if (next == '\n') {
            continue;
          }
        }
        if (flag->b == 1 && prev == '\n' && now != '\n')
          printf("%6d\t", counter_lines++);
        if (flag->n == 1 && flag->b == 0 && prev == '\n')
          printf("%6d\t", counter_lines++);
        if (flag->v == 1 && ((now >= 0 && now <= 31) || now == 127)) {
          check_flag(now, flag, &prev);
          continue;
        }
        if ((flag->e == 1 &&
             (now == '\n' || (now >= 0 && now <= 31) || now == 127))) {
          check_flag(now, flag, &prev);
          continue;
        }
        if ((flag->t == 1 &&
             (now == '\t' || (now >= 0 && now <= 31) || now == 127))) {
          check_flag(now, flag, &prev);
          continue;
        }
        if (flag->T == 1 && now == '\t') {
          check_flag(now, flag, &prev);
          continue;
        }
        if (flag->E == 1 && now == '\n') {
          check_flag(now, flag, &prev);
          continue;
        }
        putchar(now);
        prev = now;
      }
    } else {
      while ((now = fgetc(f)) != EOF) {
        putchar(now);
      }
    }
    fclose(f);
  }
}
void check_flag(char now, flags *flag, char *prev) {
  int flagOut = 0;
  if (now == 127) {
    printf("^?");
    flagOut = 1;
  }
  if ((flag->e == 1 || flag->E == 1) && now == '\n') {
    printf("$\n");
    flagOut = 1;
  }
  if ((flag->t == 1 || flag->T == 1) && now == '\t') {
    printf("^I");
    flagOut = 1;
  }
  if (flagOut == 0) {
    if (now != '\t' && now != '\n') {
      printf("^%c", now + 64);
    }
    if (flag->t == 0 && now == '\t') {
      printf("\t");
    }
    if (flag->e == 0 && now == '\n') {
      printf("\n");
    }
  }
  *prev = now;
}
