#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "s21_grep.h"

int add_pattern(grep_patterns *patterns, const char *pattern) {
  int status = 0;
  if (patterns->count == patterns->capacity) {
    size_t new_capacity = 4;

    if (patterns->capacity > 0) {
      new_capacity = patterns->capacity * 2;
    }

    char **new_items = realloc(patterns->items, new_capacity * sizeof(char *));

    if (new_items == NULL) {
      status = 1;
    } else {
      patterns->items = new_items;
      patterns->capacity = new_capacity;
    }
  }

  if (status == 0) {
    patterns->items[patterns->count] = malloc(strlen(pattern) + 1);

    if (patterns->items[patterns->count] == NULL) {
      status = 1;
    } else {
      strcpy(patterns->items[patterns->count], pattern);
      patterns->count++;
    }
  }

  return status;
}

int add_patterns_from_file(grep_patterns *patterns, const char *filename,
                           const grep_flags *flags) {
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

    while ((line_length = getline(&line, &line_capacity, file)) != -1 &&
           status == 0) {
      if (line_length > 0 && line[line_length - 1] == '\n') {
        line[line_length - 1] = '\0';
      }

      status = add_pattern(patterns, line);
    }

    free(line);
    fclose(file);
  }

  return status;
}

void free_patterns(grep_patterns *patterns) {
  for (size_t i = 0; i < patterns->count; i++) {
    free(patterns->items[i]);
  }

  free(patterns->items);

  patterns->items = NULL;
  patterns->count = 0;
  patterns->capacity = 0;
}