
WRITE_LIMT=-DWRITE_LIMIT=1000

CC=gcc
CC_FLAGS=-Wall -g
LD_FLAGS=-g
SRC=benchmark.c
OBJ=${SRC:.c=.o}
BIN=io-benchmark

all: ${BIN}

io-benchmark: benchmark.o
	${CC} ${LD_FLAGS}  -o $@ $^

separate: holes.o
	${CC} ${LD_FLAGS} -o $@ $^

%.o: %.c
	${CC} ${CC_FLAGS} ${WRITE_LIMIT} -o $@ -c $^

run: io-benchmark
	rm -f "video.mp4"
	./io-benchmark "video.mp4" 200 512 10

clean:
	rm -f *.o ${BIN} ${OBJ}
