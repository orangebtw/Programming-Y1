CFLAGS = -Wall -Wextra -ggdb
BUILDDIR = ../build

.PHONY: all

all: $(BUILDDIR)/task20.exe

$(BUILDDIR)/task20.exe: main.c
	clang $(CFLAGS) main.c -o $(BUILDDIR)/task20.exe -I SDL2/include/ -L SDL2/lib/x64/ -lSDL2 -lSDL2main -lshell32 -luser32 "-Wl,/subsystem:windows"	