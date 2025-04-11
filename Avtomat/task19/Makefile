CFLAGS = -Wall -Wextra -ggdb
BUILDDIR = ../build

.PHONY: all

all: $(BUILDDIR)/task19.exe

$(BUILDDIR)/task19.exe: main.c
	clang $(CFLAGS) main.c -I./libtomcrypt/src/headers -L ./libtomcrypt/build/Debug -l tomcrypt -o $(BUILDDIR)/task19.exe