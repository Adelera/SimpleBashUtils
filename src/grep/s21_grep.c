#include "s21_grep.h"

#include <regex.h>
#include <stdlib.h>

void bring_flags_in_front(int argc, char *argv[]);
void move_flags(char *argv[], int with_arg, int *target_index, int *i);

int main(int argc, char *argv[]) {
  int status = 0;
  grep_flags flags = {0};
  grep_patterns patterns = {NULL, 0, 0};
  grep_regexes regexes = {NULL, 0};
  int files_start_index = 0;

  bring_flags_in_front(argc, argv);
  status = parse_args(argc, argv, &flags, &patterns, &files_start_index);

  if (status == 0) {
    status = compile_regex(&regexes, &patterns, &flags);
  }

  if (status == 0) {
    status = process_files(argc, argv, files_start_index, &flags, &regexes);
  }

  free_regexes(&regexes);
  free_patterns(&patterns);

  return status;
}

void bring_flags_in_front(int argc, char *argv[]) {
  int target_index = 1;  // индекс куда нужно переставить флаг

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      int with_arg = 0;
      if ((argv[i][1] == 'e' || argv[i][1] == 'f') && argv[i][2] == '\0' &&
          (i + 1) < argc) {
        with_arg = 1;
      }

      if (i > target_index) {
        move_flags(argv, with_arg, &target_index, &i);
      } else if (with_arg) {
        target_index += 2;
        i++;
      } else {
        target_index++;
      }
    }
  }
}

void move_flags(char *argv[], int with_arg, int *target_index, int *i) {
  if (with_arg) {
    char *tmp_flag = argv[*i];
    char *tmp_arg = argv[*i + 1];

    for (int j = *i + 1; j > *target_index + 1; j--) {
      argv[j] = argv[j - 2];
    }

    argv[*target_index] = tmp_flag;
    argv[*target_index + 1] = tmp_arg;
    *target_index += 2;
    (*i)++;
  } else {
    char *tmp_flag = argv[*i];

    for (int j = *i; j > *target_index; j--) {
      argv[j] = argv[j - 1];
    }

    argv[*target_index] = tmp_flag;
    (*target_index)++;
  }
}
