#ifndef S21_CAT_H_
#define S21_CAT_H_

struct cat_flags {
  int number_nonblank;
  int number_all;
  int show_ends;
  int show_tabs;
  int squeeze_blank;
  int show_nonprinting;
};

struct cat_state {
  int line_number;
  int at_line_start;
  int empty_line_count;
};

void bring_flags_in_front(int argc, char **argv);
int parse_args(int argc, char **argv, struct cat_flags *flags);
int process_long_flags(int argc, char **argv);
int apply_flag(int opt, struct cat_flags *flags);
int process_file(const char *filename, struct cat_flags flags,
                 struct cat_state *state);
void output_char(int ch, struct cat_flags flags, struct cat_state *state);
void print_line_number(struct cat_flags flags, struct cat_state *state, int ch);
void print_nonprinting(int ch, int *char_should_print);

#endif
