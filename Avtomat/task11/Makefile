CFLAGS = -Wall -Wextra -ggdb
BUILDDIR = ../build

.PHONY: all clean

all: $(BUILDDIR)/task11.exe

$(BUILDDIR)/task11.exe: main.c $(BUILDDIR)/dynarray.o
	clang $(CFLAGS) ./main.c $(BUILDDIR)/dynarray.o -o $(BUILDDIR)/task11.exe

$(BUILDDIR)/dynarray.o: dynarray.h dynarray.c
	clang $(CFLAGS) -c ./dynarray.c -o $(BUILDDIR)/dynarray.o

clean:
	rm $(BUILDDIR)/*.o