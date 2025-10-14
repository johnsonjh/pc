################################################################################

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

################################################################################

# Environment variables supported by the "pc" build:
#   WITHOUT_LOCALE    - Disable use of localized error messages
#   WITHOUT_LONG_LONG - Disable use of 'long long' and '%lld'
#   WITH_TERNARY      - Enable ternary (base 3) output
#   WITH_BASE36       - Enable base 36 output
#   WITHOUT_ROMAN     - Enable Roman numeral output
#   WITH_STRTOK       - Enabble use of old strtok (instead of strtok_r)
#   NEED_STRFTIME     - Enable if you need an strftime implementation
#   WITHOUT_EDITOR    - Disable editor autodetection (e.g., if cross-compiling)
#   WITH_LIBEDIT      - Enable libedit (if not autodetected)
#   WITH_EDITLINE     - Enable libeditline (if not autodetected)
#   WITH_READLINE     - Enable readline (if not autodetected)
#   WITH_LINENOISE    - Enable linenoise

################################################################################
# Configuration:

PKG-CONFIG=pkg-config
RM=rm -f
XCC=$$(\
  command -v gcc 2> /dev/null || \
  command -v clang 2> /dev/null || \
  command -v c99 2> /dev/null || \
  command -v ibm-clang 2> /dev/null || \
  printf '%s\n' cc \
)

################################################################################
# DJGPP:

DJGPP_DIR=/opt/djgpp
DJGPP_ARCH=i586-pc-msdosdjgpp
CWSDSTUB=/opt/cwspdmi/cwsdstub.exe

################################################################################
# CrossMiNT:

CROSSMINT_DIR=$${HOME:-}/crossmint/usr
CROSSMINT_ARCH=m68k-atari-mintelf

################################################################################
# IA16-GCC:

IA16-GCC_DIR=$${HOME:-}/src/build-ia16/prefix
IA16-GCC_ARCH=ia16-elf

################################################################################
# Open Watcom v2:

WATCOM_DIR=/opt/watcom

################################################################################
# Amiga-GCC:

AMIGA_DIR=/opt/amiga
AMIGA_ARCH=m68k-amigaos

################################################################################

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
	if [ -n "$${WITH_STRTOK:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DWITH_STRTOK=1"; \
	fi; \
	if [ -n "$${NEED_STRFTIME:-}" ]; then \
		_CFLAGS="$${_CFLAGS:-} -DNEED_STRFTIME=1"; \
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

################################################################################

all: pc

################################################################################

clean:
	@set -x; $(RM) ./pc ./pc.exe ./pc-djgpp.exe ./pc.prg ./pc-elks ./pc-dosg.exe ./pc-dosw.exe ./pc-dosw.obj ./pc-dosw.com ./pc-doswc.obj ./pc-amiga ./pc.o

################################################################################

distclean: clean
	@set -x; $(RM) -r ./a.out ./ch.c ./compile_commands.json ./core ./log.pvs ./pvsreport
	@set -x; $(RM) -r ./core-*
	@set -x; $(RM) -r ./pc.c.out.*
	@set -x; $(RM) -r ./pc-djgpp

################################################################################

lint:
	@./.lint.sh

################################################################################

pc-djgpp.exe:
	$(RM) ./pc-djgpp ./pc-djgpp.exe
	env PATH="$(DJGPP_DIR)/$(DJGPP_ARCH)/bin:$(DJGPP_DIR)/bin:$${PATH:-}" \
		$(DJGPP_DIR)/bin/$(DJGPP_ARCH)-gcc \
			-march=i386 -Os -o ./pc-djgpp.exe ./pc.c
	$(DJGPP_DIR)/$(DJGPP_ARCH)/bin/strip ./pc-djgpp.exe
	test -f $(CWSDSTUB) && { \
		$(DJGPP_DIR)/$(DJGPP_ARCH)/bin/exe2coff ./pc-djgpp.exe && \
		$(RM) ./pc-djgpp.exe && \
		cat $(CWSDSTUB) ./pc-djgpp > ./pc-djgpp.exe && \
		$(RM) ./pc-djgpp; }

djgpp: pc-djgpp.exe

################################################################################

pc.prg:
	env PATH="$(CROSSMINT_DIR)/$(CROSSMINT_ARCH)/bin:$(CROSSMINT_DIR)/bin:$${PATH:-}" \
		$(CROSSMINT_DIR)/bin/$(CROSSMINT_ARCH)-gcc \
			-Os -o ./pc.prg ./pc.c -lgem
	$(CROSSMINT_DIR)/$(CROSSMINT_ARCH)/bin/strip ./pc.prg

atari: pc.prg

###############################################################################

pc-elks:
	env PATH="$(IA16-GCC_DIR)/$(IA16-GCC_ARCH)/bin:$(IA16-GCC_DIR)/bin:$${PATH:-}" \
		$(IA16-GCC_DIR)/bin/$(IA16-GCC_ARCH)-gcc \
			-march=i8086 -mtune=i8086 -melks -mregparmcall -Os -o ./pc-elks ./pc.c

elks: pc-elks

################################################################################

pc-dosg.exe:
	env PATH="$(IA16-GCC_DIR)/$(IA16-GCC_ARCH)/bin:$(IA16-GCC_DIR)/bin:$${PATH:-}" \
		$(IA16-GCC_DIR)/bin/$(IA16-GCC_ARCH)-gcc \
			-march=i8086 -mtune=i8086 -mcmodel=small -mregparmcall -Os -o ./pc-dosg.exe ./pc.c

gcc-dosexe: pc-dosg.exe

################################################################################

pc-dosw.exe:
	export PATH="$(WATCOM_DIR)/binl64:$${PATH:-}" && \
	export WATCOM="$(WATCOM_DIR)" && \
	export INCLUDE="$(WATCOM_DIR)/h" && \
	$(WATCOM_DIR)/binl64/wcl \
		-bcl=DOS -0 -mt -fpi -j -d0 -os -s -fo=pc-dosw.obj -fe=pc-dosw.exe pc.c && \
	$(RM) ./pc-dosw.obj

watcom-dos: pc-dosw.exe

################################################################################

pc-dosw.com:
	export PATH="$(WATCOM_DIR)/binl64:$${PATH:-}" && \
	export WATCOM="$(WATCOM_DIR)" && \
	export INCLUDE="$(WATCOM_DIR)/h" && \
	$(WATCOM_DIR)/binl64/owcc \
		-c -bt=dos -bcom -march=i86 -fsigned-char -mcmodel=t -g0 -frerun-optimizer -Os -fno-stack-check -o ./pc-doswc.obj ./pc.c && \
	$(WATCOM_DIR)/binl64/owcc \
		-bcom -s -o ./pc-dosw.com ./pc-doswc.obj && \
	$(RM) ./pc-doswc.obj

watcom-doscom: pc-dosw.com

################################################################################

pc-amiga:
	env PATH="$(AMIGA_DIR)/$(AMIGA_ARCH)/bin:$(AMIGA_DIR)/bin:$${PATH:-}" \
		$(AMIGA_DIR)/bin/$(AMIGA_ARCH)-gcc \
			-Os -o ./pc-amiga ./pc.c -noixemul
	$(AMIGA_DIR)/$(AMIGA_ARCH)/bin/strip ./pc-amiga

amiga: pc-amiga

################################################################################

everything: pc djgpp atari elks watcom-dos watcom-doscom gcc-dosexe amiga

################################################################################

.PHONY: all clean distclean lint djgpp atari elks watcom-dos watcom-doscom gcc-dosexe everything test

################################################################################
