
all: master

master: master.o blowfish.o

master.o: ../strfix.h ../trinoo.h

blowfish.o: bf_tab.h blowfish.h

clean:
	-rm -f *.o master
