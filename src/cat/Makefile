CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=201700L

TARGET = s21_cat
SRC = s21_cat.c

all: $(TARGET)

$(TARGET): $(SRC)
	 $(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	 rm -f $(TARGET)