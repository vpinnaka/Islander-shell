###########################################################
#
# Simple Makefile for Operating Systems Project 1
# guish
#
###########################################################
.SUFFIXES: .h .c .cpp .l .o

guish: guish.yy.o guish.o
	g++ guish.yy.o guish.o -o guish

guish.o: guish.cpp
	g++ -c -g guish.cpp

guish.yy.o: guish.yy.c
	gcc -c guish.yy.c

guish.yy.c: guish.l
	flex -oguish.yy.c guish.l

clean:
	/bin/rm -f *.o *~ core guish guish.yy.c





