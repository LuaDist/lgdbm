# makefile for Lua+gdbm

# change this to reflect your installation
LUA=/tmp/lhf/lua-4.0

CC= gcc
CFLAGS= $(INCS) $(DEFS) $(WARN) -O2 -g
WARN= -ansi -pedantic -Wall #-Wmissing-prototypes

INCS= -I$(LUA)/include -I$(GDBM)
LIBS= -L$(LUA)/lib -llua -llualib -lm -lgdbm

OBJS= main.o gdbm.o

T=gdbm

all:	$T

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

test:	$T
	./gdbm test.lua

clean:
	rm -f $T $(OBJS) test.gdbm core

# distribution

A=gdbm.tar.gz
TOTAR=Makefile,README,gdbm.c,gdbm.lua,main.c,repository.lua,test.lua

tar:	clean
	tar zcvf $A -C .. gdbm/{$(TOTAR)}

distr:	tar 
	mv $A ftp
