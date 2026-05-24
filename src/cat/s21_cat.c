#include "s21_cat.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  struct cat_flags flags = {0};
  struct cat_state state = {1, 1, 0};
  bring_flags_in_front(argc, argv);
  int status = parse_args(argc, argv, &flags);

  if (status == 0) {
    for (int i = optind; i < argc; i++) {
      if (process_file(argv[i], flags, &state) != 0) {
        status = 1;
      }
    }
  }

  return status;
}

void bring_flags_in_front(int argc, char **argv) {
  int target_index = 1;  // индекс куда нужно переставить флаг

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '\0' && i > target_index) {
      char *tmp_flag = argv[i];

      for (int j = i; j > target_index; j--) {
        argv[j] = argv[j - 1];
      }

      argv[target_index] = tmp_flag;
      target_index++;
    }
  }
}

int parse_args(int argc, char **argv, struct cat_flags *flags) {
  int status = 0;
  int opt;

  status = process_long_flags(argc, argv);

  while (status == 0 && (opt = getopt(argc, argv, "benstETv")) != -1) {
    status = apply_flag(opt, flags);
  }

  if (flags->number_nonblank) {
    flags->number_all = 0;
  }

  return status;
}

int process_long_flags(int argc, char **argv) {
  int status = 0;

  for (int i = 1; i < argc && status == 0; i++) {
    if (strcmp(argv[i], "--number-nonblank") == 0) {
      argv[i] = "-b";
    } else if (strcmp(argv[i], "--number") == 0) {
      argv[i] = "-n";
    } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
      argv[i] = "-s";
    } else if (strncmp(argv[i], "--", 2) == 0) {
      fprintf(stderr, "s21_cat: unknown option %s\n", argv[i]);
      status = 1;
    }
  }

  return status;
}

int apply_flag(int opt, struct cat_flags *flags) {
  int status = 0;

  switch (opt) {
    case 'b':
      flags->number_nonblank = 1;
      break;

    case 'n':
      flags->number_all = 1;
      break;

    case 's':
      flags->squeeze_blank = 1;
      break;

    case 'e':
      flags->show_ends = 1;
      flags->show_nonprinting = 1;
      break;

    case 'E':
      flags->show_ends = 1;
      break;

    case 't':
      flags->show_tabs = 1;
      flags->show_nonprinting = 1;
      break;

    case 'T':
      flags->show_tabs = 1;
      break;

    case 'v':
      flags->show_nonprinting = 1;
      break;

    default:
      status = 1;
  }

  return status;
}

int process_file(const char *filename, struct cat_flags flags,
                 struct cat_state *state) {
  int status = 0;
  FILE *file = NULL;
  int ch;

  file = fopen(filename, "r");
  if (file == NULL) {
    perror(filename);
    status = 1;
  }

  if (status == 0) {
    while ((ch = fgetc(file)) != EOF) {
      output_char(ch, flags, state);
    }
    fclose(file);
  }

  return status;
}

void output_char(int ch, struct cat_flags flags, struct cat_state *state) {
  int line_should_print = 1;
  int char_should_print = 1;

  if (state->at_line_start == 1 && flags.squeeze_blank == 1 && ch == '\n') {
    if (state->empty_line_count == 1) {
      line_should_print = 0;
    } else {
      state->empty_line_count = 1;
    }
  }

  if (state->at_line_start == 1 && ch != '\n') {
    state->empty_line_count = 0;
  }

  if (line_should_print) {
    if (flags.number_nonblank == 1 || flags.number_all == 1) {
      print_line_number(flags, state, ch);
    }

    if (flags.show_tabs == 1 && ch == '\t') {
      printf("^I");
      char_should_print = 0;
    }

    if (flags.show_nonprinting == 1) {
      print_nonprinting(ch, &char_should_print);
    }

    if (flags.show_ends == 1 && ch == '\n') {
      printf("$");
    }

    if (char_should_print == 1) {
      printf("%c", ch);
    }

    state->at_line_start = (ch == '\n');
  }
}

void print_line_number(struct cat_flags flags, struct cat_state *state,
                       int ch) {
  if (state->at_line_start == 1) {
    if (flags.number_nonblank == 1 && ch != '\n') {
      printf("%6d\t", state->line_number);
      (state->line_number)++;
    }

    if (flags.number_all == 1) {
      printf("%6d\t", state->line_number);
      (state->line_number)++;
    }
  }
}

void print_nonprinting(int ch, int *char_should_print) {
  int is_meta = 0;

  if (ch != '\n' && ch != '\t') {
    if (ch >= 128 && ch <= 255) {
      printf("M-");
      ch -= 128;
      is_meta = 1;
    }

    if (ch >= 0 && ch <= 31) {
      printf("^%c", ch + 64);
      *char_should_print = 0;
    } else if (ch == 127) {
      printf("^?");
      *char_should_print = 0;
    } else if (is_meta == 1) {
      printf("%c", ch);
      *char_should_print = 0;
    }
  }
}