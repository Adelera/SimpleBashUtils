#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "s21_grep.h"

static int process_current_file(const char *filename, const grep_flags *flags,
                                const grep_regexes *regexes, int many_files);
static void process_line(const char *filename, const char *line,
                         int line_number, const grep_flags *flags,
                         const grep_regexes *regexes, int many_files,
                         int *match_count, int *file_has_match);
static int is_line_match(const char *line, const grep_flags *flags,
                         const grep_regexes *regexes);
static void print_match(const char *filename, const char *line, int line_number,
                        const grep_flags *flags, int many_files);
static void print_only_matches(const char *filename, const char *line,
                               int line_number, const grep_flags *flags,
                               const grep_regexes *regexes, int many_files);

static int find_best_match(const char *cursor, const grep_regexes *regexes,
                           regmatch_t *out_match);

static void print_count(const char *filename, const grep_flags *flags,
                        int many_files, int match_count);

int process_files(int argc, char *argv[], int files_start_index,
                  const grep_flags *flags, const grep_regexes *regexes) {
  int status = 0;
  int many_files = (argc - files_start_index) > 1;

  for (int i = files_start_index; i < argc; i++) {
    int file_status = process_current_file(argv[i], flags, regexes, many_files);

    if (file_status != 0) {
      status = 2;
    }
  }

  return status;
}

static int process_current_file(const char *filename, const grep_flags *flags,
                                const grep_regexes *regexes, int many_files) {
  int status = 0;
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    if (flags->no_error_messages == 0) {
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    }
    status = 1;
  } else {
    char *line = NULL;
    size_t line_capacity = 0;
    ssize_t line_length = 0;
    int line_number = 0;
    int match_count = 0;
    int file_has_match = 0;

    while ((line_length = getline(&line, &line_capacity, file)) != -1) {
      line_number++;
      process_line(filename, line, line_number, flags, regexes, many_files,
                   &match_count, &file_has_match);
    }
    if (flags->show_match_count == 1 && flags->only_file_name == 0) {
      print_count(filename, flags, many_files, match_count);
    }

    if (flags->only_file_name == 1 && file_has_match == 1) {
      printf("%s\n", filename);
    }

    free(line);
    fclose(file);
  }

  return status;
}

static void process_line(const char *filename, const char *line,
                         int line_number, const grep_flags *flags,
                         const grep_regexes *regexes, int many_files,
                         int *match_count, int *file_has_match) {
  int match = is_line_match(line, flags, regexes);

  if (match) {
    (*match_count)++;
    *file_has_match = 1;

    if (flags->only_file_name == 0 && flags->show_match_count == 0) {
      if (flags->only_matching == 1 && flags->invert_match == 0) {
        print_only_matches(filename, line, line_number, flags, regexes,
                           many_files);
      } else if (flags->only_matching == 0) {
        print_match(filename, line, line_number, flags, many_files);
      }
    }
  }
}

static int is_line_match(const char *line, const grep_flags *flags,
                         const grep_regexes *regexes) {
  int match = 0;

  for (size_t i = 0; i < regexes->count && match == 0; i++) {
    if (regexec(&regexes->items[i], line, 0, NULL, 0) == 0) {
      match = 1;
    }
  }

  if (flags->invert_match == 1) {
    match = !match;
  }

  return match;
}

static void print_match(const char *filename, const char *line, int line_number,
                        const grep_flags *flags, int many_files) {
  if (many_files && flags->no_filename == 0) {
    printf("%s:", filename);
  }

  if (flags->line_number == 1) {
    printf("%d:", line_number);
  }

  if (strlen(line) > 0 && line[strlen(line) - 1] != '\n') {
    printf("%s\n", line);
  } else {
    printf("%s", line);
  }
}

static void print_only_matches(const char *filename, const char *line,
                               int line_number, const grep_flags *flags,
                               const grep_regexes *regexes, int many_files) {
  const char *cursor = line;
  regmatch_t match;

  while (find_best_match(cursor, regexes, &match)) {
    if (many_files && !flags->no_filename) printf("%s:", filename);
    if (flags->line_number) printf("%d:", line_number);
    printf("%.*s\n", (int)(match.rm_eo - match.rm_so), cursor + match.rm_so);
    cursor += match.rm_eo;
  }
}

static int find_best_match(const char *cursor, const grep_regexes *regexes,
                           regmatch_t *out_match) {
  int found = 0;

  for (size_t i = 0; i < regexes->count; i++) {
    regmatch_t current;
    if (regexec(&regexes->items[i], cursor, 1, &current, 0) == 0 &&
        current.rm_so != current.rm_eo) {
      *out_match = current;
      found = 1;
    }
  }
  return found;
}

static void print_count(const char *filename, const grep_flags *flags,
                        int many_files, int match_count) {
  if (many_files && flags->no_filename == 0) {
    printf("%s:", filename);
  }

  printf("%d\n", match_count);
}