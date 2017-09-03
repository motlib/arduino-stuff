

TARGET=diagtest

SRC=$(wildcard *.c)
OBJS=${SRC:.c=.o}

$(warning objs ${OBJS})

all: ${TARGET}

${TARGET}: ${OBJS}
	gcc -o $@ ${OBJS}

%.o: %.c
	gcc -c $<

clean:
	rm -f ${OBJS}
	rm -f ${TARGET}
