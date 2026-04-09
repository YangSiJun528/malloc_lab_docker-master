#
# Students' Makefile for the Malloc Lab
#
TEAM = bovik
VERSION = 1
HANDINDIR = /afs/cs.cmu.edu/academic/class/15213-f01/malloclab/handin

CC = gcc
# CFLAGS = -Wall -O2 -m32
CFLAGS = -Wall -O2 -g

OBJS = mdriver.o mm.o memlib.o fsecs.o fcyc.o clock.o ftimer.o

.PHONY: asan-ubsan valgrind-run

mdriver: $(OBJS)
	$(CC) $(CFLAGS) -o mdriver $(OBJS)

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h
mm.o: mm.c mm.h memlib.h
fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h

handin:
	cp mm.c $(HANDINDIR)/$(TEAM)-$(VERSION)-mm.c

asan-ubsan:
	$(MAKE) clean
	$(MAKE) CFLAGS='-Wall -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer'
	UBSAN_OPTIONS=print_stacktrace=1 ./mdriver -V -f short1-bal.rep

valgrind-run:
	$(MAKE) clean
	$(MAKE) CFLAGS='-Wall -O0 -g'
	valgrind --leak-check=full --track-origins=yes ./mdriver -V -f short1-bal.rep

clean:
	rm -f *~ *.o mdriver

