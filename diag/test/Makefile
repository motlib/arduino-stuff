

TARGET=diagtest

SRC=$(wildcard *.c) $(wildcard ../*.c)
OBJS=${SRC:.c=.o}

$(warning objs ${OBJS})

all: ${TARGET}

${TARGET}: ${OBJS}
	gcc -o $@ ${OBJS}

%.o: %.c
	gcc -I.. -c $< -o $@

clean:
	rm -f ${OBJS}
	rm -f ${TARGET}
