RM=rm -f
CC=$$(command -v gcc 2>/dev/null || command -v clang 2>/dev/null || command -v c99 2>/dev/null || printf '%s\n' cc || :)

all: pc

pc: pc.c
	case "$$(uname -s 2>/dev/null || :)" in AIX) export OBJECT_MODE=64; case $(CC) in *gcc*) CFLAGS="$(CFLAGS) -maix64" ;; esac ;; esac; $(CC) $(CFLAGS) $(LDFLAGS) pc.c -o pc

clean:
	$(RM) pc

.PHONY: all clean test
