# SPDX-License-Identifier: MIT
# Copyright (c) 1993 Dominic Giampaolo <dbg@be.com>
# Copyright (c) 1994 Joel Tesler <joel@engr.sgi.com>
# Copyright (c) 2005 Axel Dörfler <axeld@pinc-software.de>
# Copyright (c) 2005 Ingo Weinhold <ingo_weinhold@gmx.de>
# Copyright (c) 2009 Oliver Tappe <zooey@hirschkaefer.de>
# Copyright (c) 2017 Tuan Kiet Ho <tuankiet65@gmail.com>
# Copyright (c) 2019 Adrien Destugues <pulkomandy@pulkomandy.tk>
# Copyright (c) 2022-2025 Jeffrey H. Johnson <trnsz@pobox.com>
# Copyright (c) 2022-2025 The DPS8M Development Team
# scspell-id: 805354da-a39e-11f0-8637-80ee73e9b8e7

# Environment variables supported by the build:
#   WITHOUT_LOCALE    - Disable use of localized error messages
#   WITHOUT_LONG_LONG - Disable use of 'long long' and '%lld'
#   WITH_TERNARY      - Enable ternary (base 3) output
#   WITH_BASE36       - Enable base 36 output
#   WITHOUT_ROMAN     - Enable Roman numeral output
#   WITHOUT_EDITOR    - Disable editor autodetection (e.g., if cross-compiling)
#   WITH_LIBEDIT      - Enable libedit (if not autodetected)
#   WITH_EDITLINE     - Enable libeditline (if not autodetected)
#   WITH_READLINE     - Enable readline (if not autodetected)
#   WITH_LINENOISE    - Enable linenoise

PKG-CONFIG=pkg-config
RM=rm -f
XCC=$$(\
  command -v gcc 2> /dev/null || \
  command -v clang 2> /dev/null || \
  command -v c99 2> /dev/null || \
  command -v ibm-clang 2> /dev/null || \
  printf '%s\n' cc \
)

pc: pc.c
	@XCC="$(XCC)"; \
	_CFLAGS="$(CFLAGS)"; \
	_LDFLAGS="$(LDFLAGS)"; \
	if [ -n "$${WITHOUT_LOCALE:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITHOUT_LOCALE=1"; \
	fi; \
	if [ -n "$${WITHOUT_LONG_LONG:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITHOUT_LONG_LONG=1"; \
	fi; \
	if [ -n "$${WITH_TERNARY:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITH_TERNARY=1"; \
	fi; \
	if [ -n "$${WITH_BASE36:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITH_BASE36=1"; \
	fi; \
	if [ -n "$${WITHOUT_ROMAN:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITHOUT_ROMAN=1"; \
	fi; \
	if [ -n "$${WITHOUT_EDITOR:-}" ]; then \
		_HAVE_RL=1; \
	fi; \
	if [ -n "$${WITH_LIBEDIT:-}" ]; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_LIBEDIT=1"; \
		_LDFLAGS="$${_LDFLAGS:-} -ledit"; \
	fi; \
	if [ -n "$${WITH_EDITLINE:-}" ]; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_EDITLINE=1"; \
		_LDFLAGS="$${_LDFLAGS:-} -leditline"; \
	fi; \
	if [ -n "$${WITH_READLINE:-}" ]; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_READLINE=1"; \
		_LDFLAGS="$${_LDFLAGS:-} -lreadline"; \
	fi; \
	if [ -n "$${WITH_LINENOISE:-}" ]; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_LINENOISE=1"; \
		_LDFLAGS="$${_LDFLAGS:-} -llinenoise"; \
	fi; \
	if [ "$${_HAVE_RL:-0}" -ne 1 ] && $(PKG-CONFIG) --cflags --libs libedit > /dev/null 2>&1; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_LIBEDIT=1 $$($(PKG-CONFIG) --cflags libedit 2> /dev/null)"; \
		_LDFLAGS="$${_LDFLAGS:-} $$($(PKG-CONFIG) --libs libedit 2> /dev/null)"; \
	fi; \
	if [ "$${_HAVE_RL:-0}" -ne 1 ] && $(PKG-CONFIG) --cflags --libs libeditline > /dev/null 2>&1; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_EDITLINE=1 $$($(PKG-CONFIG) --cflags libeditline 2> /dev/null)"; \
		_LDFLAGS="$${_LDFLAGS:-} $$($(PKG-CONFIG) --libs libeditline 2> /dev/null)"; \
	fi; \
	if [ "$${_HAVE_RL:-0}" -ne 1 ] && $(PKG-CONFIG) --cflags --libs readline > /dev/null 2>&1; then \
		_HAVE_RL=1; \
		_CFLAGS="$${_CFLAGS:-} -DWITH_READLINE=1 $$($(PKG-CONFIG) --cflags readline 2> /dev/null)"; \
		_LDFLAGS="$${_LDFLAGS:-} $$($(PKG-CONFIG) --libs readline 2> /dev/null)"; \
	fi; \
	case "$$(uname -s 2> /dev/null || :)" in AIX) OM="OBJECT_MODE=64"; ;; esac; \
	test "$$(command -v "$${CC:-}" 2> /dev/null)" && { XCC="$${CC:-}"; }; \
	test "$${OM:-}" && case "$${XCC:?}" in *gcc*) _CFLAGS="$${_CFLAGS:-} -maix64"; ;; esac; \
	set -x; \
	env $${OM:-} $${XCC:?} $${_CFLAGS:-} $${_LDFLAGS} pc.c -o pc

all: pc

clean:
	@set -x; $(RM) ./pc ./pc.com ./pc.exe ./pc.o

distclean: clean
	@set -x; $(RM) -r ./a.out ./ch.c ./compile_commands.json ./core ./log.pvs ./pvsreport
	@set -x; $(RM) -r ./core-*
	@set -x; $(RM) -r ./pc.c.out.*

lint:
	@./.lint.sh

.PHONY: all clean distclean lint test
