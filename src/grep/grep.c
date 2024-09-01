#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1024
#define LITTLE_SIZE 128

typedef struct flags {
  char v, i, o, l, n, c, e, f, s, h;
} flags;

enum error_codes {
  OK = 0,
  NO_MATCHES_FOUND = 1,
  ERROR = 2,
};

enum flag_codes {
  CLEAR = 0,
  SET = 1,
};

int init_struct(flags* flag, int symbol, char* template);
void init_template(char* template, const char* src);
int executor(const char** argv, const char* template, flags const* flag);
int file_counter(const char** argv, int flag_no_template_opt);
int file_handler(const char** argv, const char* template, int num_files,
                 int flag_no_template_opt, flags const* flag);
int opt_handler(const char* file_name, int num_files, int num_str,
                char* buf_str, const char* template, flags const* flag);
int c_handler(flags const* flag, int num_files, const char* file_name,
              unsigned int num_matching_strings);
int o_handler(flags const* flag, char* buffer, const char* template);
int f_handler(char* template);

int main(int argc, char** argv) {
  int errcode = NO_MATCHES_FOUND;

  if (argc > 2) {
    flags flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char template[SIZE] = {0};
    int opt_symbol = 0;
    char* optstring = "violnce:f:sh?";

    while (-1 != (opt_symbol = getopt_long(argc, argv, optstring, 0, NULL))) {
      errcode = init_struct(&flag, opt_symbol, template);
    }
    if ((flag.e || flag.f) & (argc < 4)) {
      errcode = ERROR;
    }
    if (ERROR != errcode) {
      executor((const char**)argv, template, &flag);
    }
  }
  return errcode;
}

int init_struct(flags* flag, int symbol, char* template) {
  int errcode = OK;

  switch (symbol) {
    case 'v':
      flag->v = SET;
      break;
    case 'i':
      flag->i = SET;
      break;
    case 'o':
      flag->o = SET;
      break;
    case 'l':
      flag->l = SET;
      break;
    case 'n':
      flag->n = SET;
      break;
    case 'c':
      flag->c = SET;
      break;
    case 'e':
      flag->e = SET;
      init_template(template, optarg);
      break;
    case 'f':
      flag->f = SET;
      errcode = f_handler(template);
      break;
    case 's':
      flag->s = SET;
      break;
    case 'h':
      flag->h = SET;
      break;
    case '?':
      errcode = ERROR;
  }
  return errcode;
}

void init_template(char* template, const char* string) {
  if (0 == *string) {
    strcpy(template, ".*\0");
  }

  if (0 == template[0]) {
    strcpy(template, string);
  } else if (strcmp(template, ".*") != 0) {
    strcat(template, "|");
    strcat(template, string);
  }
}

int executor(const char** argv, const char* template, flags const* flag) {
  int errcode = NO_MATCHES_FOUND;
  int num_files = 0;
  int flag_no_template_opt = CLEAR;

  if (flag->e || flag->f) {
    num_files = file_counter(argv, flag_no_template_opt);
  } else {
    flag_no_template_opt = SET;
    template = argv[optind];
    num_files = file_counter(argv, flag_no_template_opt);
  }

  errcode = file_handler(argv, template, num_files, flag_no_template_opt, flag);

  return errcode;
}

int file_counter(const char** argv, int flag_no_template_opt) {
  int num_files = 0;
  int ind = optind;

  if (flag_no_template_opt) {
    ind += 1;
  }
  for (int i = ind; NULL != argv[i]; ++i) {
    if (argv[i][0] != '-') {
      num_files += 1;
    }
  }
  return num_files;
}

int file_handler(const char** argv, const char* template, int num_files,
                 int flag_no_template_opt, flags const* flag) {
  int errcode = NO_MATCHES_FOUND;
  for (int index_loop = 0; index_loop < num_files; ++index_loop) {
    FILE* file_ptr;
    int ind_file_arg = optind + index_loop + flag_no_template_opt;
    const char* file_name = argv[ind_file_arg];
    if (NULL == (file_ptr = fopen(file_name, "r"))) {
      if (!flag->s) {
        fprintf(stderr, "s21_grep: %s: %s\n", file_name, strerror(2));
      }
      errcode = ERROR;
    } else {
      char opt_l_handling_is = CLEAR;
      regex_t preg;
      unsigned int num_matching_strings = 0;
      int regcode = flag->i ? regcomp(&preg, template, REG_ICASE)
                            : regcomp(&preg, template, REG_EXTENDED);
      if (OK != regcode) {
        char reg_errbuf[LITTLE_SIZE] = {0};
        regerror(regcode, &preg, reg_errbuf, LITTLE_SIZE);
        fprintf(stderr, "Regexp compilation failed: '%s'\n", reg_errbuf);
        errcode = ERROR;
      }
      if (ERROR != errcode) {
        char buf_str[SIZE] = {0};
        for (int num_str = 1; NULL != fgets(buf_str, SIZE, file_ptr);
             ++num_str) {
          if ((!flag->v && (regexec(&preg, buf_str, 0, NULL, 0) == OK)) ||
              (flag->v && (regexec(&preg, buf_str, 0, NULL, 0) != OK))) {
            if (flag->c) {
              flag->l ? num_matching_strings = 1 : ++num_matching_strings;
            }
            if (flag->l) {
              opt_l_handling_is = SET;
            } else {
              errcode = opt_handler(file_name, num_files, num_str, buf_str,
                                    template, flag);
            }
          }
        }
      }
      if (flag->c) {
        errcode = c_handler(flag, num_files, file_name, num_matching_strings);
      }
      if (opt_l_handling_is == SET) {
        printf("%s\n", file_name);
        errcode = OK;
      }
      regfree(&preg);
      fclose(file_ptr);
    }
  }
  return errcode;
}

int opt_handler(const char* file_name, int num_files, int num_str,
                char* buf_str, const char* template, flags const* flag) {
  int errcode = NO_MATCHES_FOUND;

  if (!flag->c) {
    if (num_files > 1 && !flag->h) {
      printf("%s:", file_name);
    }
    if (flag->n) {
      printf("%d:", num_str);
    }

    if (flag->o && !flag->v) {
      errcode = o_handler(flag, buf_str, template);
    } else {
      fputs(buf_str, stdout);
      errcode = OK;
    }

    if (!flag->o) {
      int n = strlen(buf_str);
      if (buf_str[n] == '\0' && buf_str[n - 1] != '\n') {
        putchar('\n');
      }
    }
  }
  return errcode;
}

int c_handler(flags const* flag, int num_files, const char* file_name,
              unsigned int num_matching_strings) {
  int errcode = NO_MATCHES_FOUND;

  if ((num_files > 1) && !flag->h) {
    printf("%s:%u\n", file_name, num_matching_strings);
    errcode = OK;
  } else {
    printf("%u\n", num_matching_strings);
    errcode = OK;
  }

  return errcode;
}

int o_handler(flags const* flag, char* buf_str, const char* template) {
  int errcode = NO_MATCHES_FOUND;
  regex_t preg;
  int regcode = flag->i ? regcomp(&preg, template, REG_ICASE)
                        : regcomp(&preg, template, REG_EXTENDED);

  if (OK != regcode) {
    char reg_errbuf[LITTLE_SIZE] = {0};

    regerror(regcode, &preg, reg_errbuf, LITTLE_SIZE);
    fprintf(stderr, "Regexp compilation failed: '%s'\n", reg_errbuf);
    errcode = ERROR;
  }

  if (OK == regcode && !flag->v) {
    regmatch_t pmatch[SIZE];
    char* s = buf_str;

    for (int i = 0; buf_str[i] != '\0'; ++i) {
      if (0 != regexec(&preg, s, 1, pmatch, 0)) {
        break;
      }
      printf("%.*s\n", (int)(pmatch->rm_eo - pmatch->rm_so), s + pmatch->rm_so);
      s += pmatch->rm_eo;
      errcode = OK;
    }
  }
  regfree(&preg);
  return errcode;
}

int f_handler(char* template) {
  int errcode = NO_MATCHES_FOUND;

  FILE* file_template_pointer;
  const char* file_name_template = optarg;

  if (NULL == (file_template_pointer = fopen(file_name_template, "r"))) {
    fprintf(stderr, "s21_grep: %s: %s\n", file_name_template, strerror(2));
    errcode = ERROR;
  } else {
    char buf_str_template[SIZE] = {0};

    while (NULL != fgets(buf_str_template, SIZE, file_template_pointer)) {
      if ('\n' == *buf_str_template) {
        strcpy(template, ".*\0");
      } else {
        buf_str_template[strlen(buf_str_template) - 1] = '\0';
        init_template(template, buf_str_template);
      }
    }
    fclose(file_template_pointer);
  }
  return errcode;
}