# Generated automatically from Makefile.in by configure.

#DEBUG	= -g -DDEBUG

CC	= gcc
PREFIX	= /usr/local
BINDIR	= /usr/local/bin
MANDIR	= /usr/local/man/man1

DEFINES = -DHAVE_CONFIG_H -DCFG_UNIX_API
CFLAGS	= -Wall -O3 ${DEBUG} ${DEFINES}


OBJS	= main.o rename.o fixtoken.o
TARGET	= renamex
MANPAGE	= renamex.1

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	cp $@ /usr/local/bin

static:	$(OBJS)
	$(CC) $(CFLAGS) -static -o $@ $^

.PHONY: clean clean-all install
clean:
	rm -f $(TARGET) $(OBJS)

clean-all: clean
	rm -f config.status config.cache config.h config.log Makefile

install:
	install -o root -g root -m 0755 -s $(TARGET) $(BINDIR)
	install -o root -g root -m 0644 $(MANPAGE) $(MANDIR)
	
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<


