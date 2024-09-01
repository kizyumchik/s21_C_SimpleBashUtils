#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags {
  char b, e, n, s, t, v;
} flags;

enum error_codes {
  OK = 0,
  ERROR = 1,
};

enum flag_codes {
  CLEAR = 0,
  SET = 1,
};

enum position_codes { IS_BEGIN = 0, IS_MID = 1 };

int file_exist(FILE* file);
int check_long(char* check_f, flags* flag);
int check_short(char check_f, flags* flag);
int flag_parser(int argc, char** argv, flags* flag);
int file_handler(char* file_name, flags* flag);
void b_handler(FILE* file, const int* c, flags* flag, unsigned int* num_str,
               const char* position);
void n_handler(FILE* file, const int* c, flags* flag, unsigned int* num_str);
void s_handler(FILE* file, int* c, flags* flag, unsigned int* num_str,
               char* position);
void s_flag_handler(FILE* file, int* fut_c, int* c, flags* flag,
                    unsigned int* num_str, const char* position);
void v_handler(int* c, flags* flag);
void t_handler(int* c, flags* flag);
void e_handler(int* c, flags* flag);

int main(int argc, char** argv) {
  int errcode = OK;
  flags flag = {0, 0, 0, 0, 0, 0};

  errcode = flag_parser(argc, argv, &flag);
  for (int i = 1; (i < argc) && (errcode == OK); i++) {
    if (argv[i][0] != '-') {
      errcode = file_handler(argv[i], &flag);
    }
  }
  return errcode;
}

int check_long(char* check_f, flags* flag) {
  int errcode = ERROR;
  if (strcmp(check_f, "number-nonblank") == 0) {
    flag->b = SET;
    errcode = OK;
  } else if (strcmp(check_f, "number") == 0) {
    flag->n = SET;
    errcode = OK;
  } else if (strcmp(check_f, "squeeze-blank") == 0) {
    flag->s = SET;
    errcode = OK;
  }
  return errcode;
}

int check_short(char check_f, flags* flag) {
  int errcode = OK;
  switch (check_f) {
    case 'b':
      flag->b = SET;
      break;
    case 'e':
      flag->v = flag->e = SET;
      break;
    case 'E':
      flag->e = SET;
      break;
    case 'n':
      flag->n = SET;
      break;
    case 's':
      flag->s = SET;
      break;
    case 't':
      flag->v = flag->t = SET;
      break;
    case 'T':
      flag->t = SET;
      break;
    case 'v':
      flag->v = SET;
      break;
    default:
      errcode = ERROR;
      break;
  }
  return errcode;
}

int flag_parser(int argc, char** argv, flags* flag) {
  int errcode = OK;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (check_long(&argv[i][2], flag) != OK) {
          fprintf(stderr, "s21_cat: unrecognized option --%s\n", &argv[i][2]);
          errcode = ERROR;
        }
      } else {
        char* f = &argv[i][1];
        if (check_short(*f, flag) != OK) {
          fprintf(stderr, "s21_cat: invalid option -- '%s'\n", f);
          errcode = ERROR;
        }
      }
    }
  }
  return errcode;
}

int file_handler(char* file_name, flags* flag) {
  FILE* file_ptr;
  int errcode = OK;

  if (NULL == (file_ptr = fopen(file_name, "r"))) {
    fprintf(stderr, "s21_cat: %s: %s\n", file_name, strerror(2));
    errcode = ERROR;
  } else {
    int c = fgetc(file_ptr);

    if (file_exist(file_ptr)) {
      unsigned int num_str = 0;
      char flag_position = IS_BEGIN;

      b_handler(file_ptr, &c, flag, &num_str, &flag_position);
      n_handler(file_ptr, &c, flag, &num_str);
      s_handler(file_ptr, &c, flag, &num_str, &flag_position);

      flag_position = IS_MID;
      do {
        v_handler(&c, flag);
        t_handler(&c, flag);
        s_handler(file_ptr, &c, flag, &num_str, &flag_position);
        e_handler(&c, flag);

        putchar(c);

        b_handler(file_ptr, &c, flag, &num_str, &flag_position);
        n_handler(file_ptr, &c, flag, &num_str);
        c = fgetc(file_ptr);
      } while (file_exist(file_ptr));
    }
    fclose(file_ptr);
  }
  return errcode;
}

int file_exist(FILE* file_ptr) {
  int file_status = ERROR;

  if (0 != feof(file_ptr)) {
    file_status = OK;
  } else if (0 != ferror(file_ptr)) {
    file_status = OK;
  }
  return file_status;
}

void b_handler(FILE* file_ptr, const int* c, flags* flag, unsigned int* num_str,
               const char* flag_position) {
  if (flag->b) {
    int fut_c = fgetc(file_ptr);

    flag->n = 0;
    if (file_exist(file_ptr)) {
      if ((*c == '\n' && fut_c != '\n' && *flag_position == IS_MID) ||
          (*c != '\n' && *flag_position == IS_BEGIN)) {
        printf("%6u\t", ++(*num_str));
      }
      fseek(file_ptr, -1, SEEK_CUR);
    }
  }
}

void n_handler(FILE* file_ptr, const int* c, flags* flag,
               unsigned int* num_str) {
  if (flag->n) {
    fgetc(file_ptr);
    if (file_exist(file_ptr)) {
      if (*c == '\n' || *num_str == 0) {
        printf("%6u\t", ++(*num_str));
      }
      fseek(file_ptr, -1, SEEK_CUR);
    }
  }
}

void s_flag_handler(FILE* file_ptr, int* fut_c, int* c, flags* flag,
                    unsigned int* num_str, const char* flag_position) {
  if (*fut_c == '\n') {
    if (flag->e) {
      printf("$");
    }
    while (*fut_c == '\n') {
      *fut_c = fgetc(file_ptr);
    }
    putchar('\n');
    if (*flag_position == IS_BEGIN) {
      *c = *fut_c;
    }
    if (flag->n || (flag->b && *flag_position == IS_BEGIN)) {
      printf("%6u\t", ++(*num_str));
    }
  }
}

void s_handler(FILE* file_ptr, int* c, flags* flag, unsigned int* num_str,
               char* flag_position) {
  if (flag->s) {
    int fut_c = fgetc(file_ptr);

    if (*flag_position == IS_BEGIN) {
      if (*c != '\n') {
        fseek(file_ptr, -1, SEEK_CUR);
      }
      s_flag_handler(file_ptr, &fut_c, c, flag, num_str, flag_position);
    } else if (*flag_position == IS_MID) {
      if (*c == '\n') {
        s_flag_handler(file_ptr, &fut_c, c, flag, num_str, flag_position);
      }
      if (file_exist(file_ptr)) {
        fseek(file_ptr, -1, SEEK_CUR);
      }
    }
  }
}

void v_handler(int* c, flags* flag) {
  if (flag->v) {
    if ((0 <= *c && *c <= 8) || (11 <= *c && *c <= 31)) {
      putchar('^');
      *c += 64;
    }
    if (*c == 127) {
      putchar('^');
      *c -= 64;
    }
  }
}

void t_handler(int* c, flags* flag) {
  if (flag->t && *c == '\t') {
    putchar('^');
    *c += 64;
  }
  v_handler(c, flag);
}

void e_handler(int* c, flags* flag) {
  if (flag->e && (*c == '\n')) {
    printf("$");
  }
  v_handler(c, flag);
}