CFLAGS = -Wall -Wextra -ggdb
BUILDDIR = ../build

.PHONY: all

$(BUILDDIR)/task18.exe: main.c
	clang $(CFLAGS) main.c -I ./gsl/build/gsl -L ./gsl/build/Release/ -lgsl -lgslcblas -o $(BUILDDIR)/task18.exe