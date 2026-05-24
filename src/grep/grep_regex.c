#include <regex.h>
#include <stdlib.h>

#include "s21_grep.h"

int compile_regex(grep_regexes *regexes, const grep_patterns *patterns,
                  const grep_flags *flags) {
  int status = 0;
  int compile_flags = 0;

  regexes->items = NULL;
  regexes->count = 0;

  if (flags->ignore_case == 1) {
    compile_flags |= REG_ICASE;
  }

  regexes->items = calloc(patterns->count, sizeof(regex_t));

  if (regexes->items == NULL) {
    status = 1;
  }

  for (size_t i = 0; status == 0 && i < patterns->count; i++) {
    if (regcomp(&regexes->items[i], patterns->items[i], compile_flags) != 0) {
      status = 1;
    } else {
      regexes->count++;
    }
  }

  return status;
}

void free_regexes(grep_regexes *regexes) {
  for (size_t i = 0; i < regexes->count; i++) {
    regfree(&regexes->items[i]);
  }

  free(regexes->items);
  regexes->items = NULL;
  regexes->count = 0;
}