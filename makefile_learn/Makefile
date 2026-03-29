CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra --pedantic -Wvla -Wshadow -Werror
TESTFLAGS = -DASCENDING

SRCS = main.c sort.c
OBJ = $(SRCS:.c=.o)
EXEC = sort

all: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(EXEC)


## use echo "Hello" | diff - file.c 
# the - means to read from standard input
# -w means ignore whitespaces