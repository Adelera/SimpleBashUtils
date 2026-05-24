#ifndef S21_GREP_H_
#define S21_GREP_H_

#include <regex.h>
#include <stddef.h>

typedef struct grep_flags {
  int ignore_case;
  int invert_match;
  int show_match_count;
  int only_file_name;
  int line_number;
  int no_filename;
  int no_error_messages;
  int only_matching;
} grep_flags;

typedef struct grep_patterns {
  char **items;
  size_t count;
  size_t capacity;
} grep_patterns;

typedef struct grep_regexes {
  regex_t *items;
  size_t count;
} grep_regexes;

int parse_args(int argc, char *argv[], grep_flags *flags,
               grep_patterns *patterns, int *files_start_index);
int add_pattern(grep_patterns *patterns, const char *pattern);
int add_patterns_from_file(grep_patterns *patterns, const char *filename,
                           const grep_flags *flags);
void free_patterns(grep_patterns *patterns);

int compile_regex(grep_regexes *regexes, const grep_patterns *patterns,
                  const grep_flags *flags);

void free_regexes(grep_regexes *regexes);

int process_files(int argc, char *argv[], int files_start_index,
                  const grep_flags *flags, const grep_regexes *regexes);

#endif
