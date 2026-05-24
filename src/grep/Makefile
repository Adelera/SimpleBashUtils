CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=201700L

TARGET = s21_grep
SRC = s21_grep.c grep_parser.c grep_patterns.c grep_regex.c grep_process.c

all: $(TARGET)

$(TARGET): $(SRC)
	 $(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	 rm -f $(TARGET)