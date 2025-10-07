RM=rm -f
CC=$$(command -v gcc 2>/dev/null || command -v clang 2>/dev/null || command -v c99 2>/dev/null || printf '%s\n' cc || :)

pc: pc.c
	export CC="$(CC)"; export CFLAGS="$(CFLAGS)"; case "$$(uname -s 2>/dev/null || :)" in AIX) export OBJECT_MODE=64; case "$${CC:?}" in *gcc*) export CFLAGS="$(CFLAGS) -maix64" ;; esac ;; esac; $${CC:?} $${CFLAGS:-} $(LDFLAGS) pc.c -o pc

all: pc

clean:
	$(RM) ./pc

distclean: clean
	$(RM) -r ./a.out ./compile_commands.json ./core ./log.pvs ./pvsreport
	$(RM) -r ./pc.c.out.*

lint:
	@./.lint.sh

.PHONY: all clean distclean lint test
