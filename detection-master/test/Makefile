
CFLAGS=-Wall -Wextra

all: test

test: test.o
	gcc -g -o $@ $<

%.o: %.c
	gcc ${CFLAGS} -o $@ -c $<


