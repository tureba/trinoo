LDFLAGS=-lcrypt

CFLAGS=-O2 -g -ggdb -Wall -Wextra

all: master

master: master.o blowfish.o

master.o: strfix.h

blowfish.o: bf_tab.h blowfish.h

clean:
	-rm -f *.o master
