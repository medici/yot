CC = gcc
AS=	as
AR=	ar -rc
RANLIB=	ranlib
SRC = printt.c misc.c error.c scan.c base.c generator.c generate_code.c parser.c main.c

all: yot base lib libyot.a
	
yot: $(SRC)
	$(CC) -o yot $(SRC)

base: 
	$(AS) -o lib/libs.o  lib/libs.s 

SRCS =$(wildcard ./lib/*.yot)
OBJS =$(patsubst %.yot,%.o,$(SRCS))
lib:	$(OBJS)
%.o:%.yot
	./yot -c  $<

libyot.a:	$(OBJS)
	$(AR) lib/libyot.a $(OBJS)
	$(RANLIB) lib/libyot.a

c:
	as -arch x86_64 -mmacosx-version-min=10.10 -g -o test.o test.s && ld -lc -arch x86_64 -macosx_version_min 10.10 -o test test.o lib/libs.o  lib/libyot.a