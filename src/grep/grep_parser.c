#include <unistd.h>

#include "s21_grep.h"

static int apply_flags(int opt, char *optarg_value, grep_flags *flags,
                       grep_patterns *patterns);

int parse_args(int argc, char *argv[], grep_flags *flags,
               grep_patterns *patterns, int *files_start_index) {
  int status = 0;
  int opt;

  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && status == 0) {
    status = apply_flags(opt, optarg, flags, patterns);
  }

  if (status == 0 && patterns->count == 0 && optind < argc) {
    status = add_pattern(patterns, argv[optind]);
    optind++;
  }

  if (status == 0 && patterns->count == 0) {
    status = 1;
  }

  if (status == 0) {
    *files_start_index = optind;
  }

  return status;
}

static int apply_flags(int opt, char *optarg_value, grep_flags *flags,
                       grep_patterns *patterns) {
  int status = 0;

  switch (opt) {
    case 'e':
      status = add_pattern(patterns, optarg_value);
      break;
    case 'i':
      flags->ignore_case = 1;
      break;
    case 'v':
      flags->invert_match = 1;
      break;
    case 'c':
      flags->show_match_count = 1;
      break;
    case 'l':
      flags->only_file_name = 1;
      break;
    case 'n':
      flags->line_number = 1;
      break;
    case 'h':
      flags->no_filename = 1;
      break;
    case 's':
      flags->no_error_messages = 1;
      break;
    case 'f':
      status = add_patterns_from_file(patterns, optarg_value, flags);
      break;
    case 'o':
      flags->only_matching = 1;
      break;
    default:
      status = 1;
      break;
  }
  return status;
}