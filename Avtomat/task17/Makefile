CFLAGS = -Wall -Wextra -ggdb -DCURL_STATICLIB
BUILDDIR = ../build

.PHONY: all

all: $(BUILDDIR)/task17.exe

$(BUILDDIR)/task17.exe: main.c
	clang $(CFLAGS) main.c -L ./curl/build/lib/Release -I ./curl/include/ -L C:/Users/roman/scoop/apps/openssl/current/lib/VC/x64/MT -l libcurl -l user32 -lwldap32 -lws2_32 -l libssl_static -l advapi32 -l crypt32 -l libcrypto_static  -o $(BUILDDIR)/task17.exe