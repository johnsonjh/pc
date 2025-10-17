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
#   WITH_STRTOK       - Enable use of old strtok (instead of strtok_r)
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

RETRO68_DIR=$${HOME:-}/r68/Retro68-build/toolchain
RETRO68_68K_ARCH=m68k-apple-macos
RETRO68_PPC_ARCH=powerpc-apple-macos
RETRO68_REZ=Rez
RETRO68_MAKEPEF=MakePEF

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
	env $${OM:-} $${XCC:?} $${_CFLAGS:-} $(EXTRA_CFLAGS) $${_LDFLAGS} $(EXTRA_LDFLAGS) pc.c -o pc

################################################################################

all: pc

################################################################################

clean:
	@set -x; $(RM) ./pc ./pc.exe ./pc-djgpp.exe ./pc.tos ./pc-elks ./pc-dosg.exe ./pc-dosw.exe ./pc-dosw.obj ./pc-dosw.com ./pc-doswc.obj ./pc-amiga ./pc.o ./pc-mac68k ./pc-mac68k.bin ./pc-mac68k.o ./pc-mac68k.gdb ./pc-mac68k.dsk ./pc-macppc.dsk ./pc-macppc.pef ./pc-mac68k.bin.gdb ./dpsprintf.o ./pc-macppc.bin ./pc-macppc.pef ./pc-macppc.o ./extra.h ./rez.r

################################################################################

distclean: clean
	@set -x; $(RM) -r ./a.out ./ch.c ./compile_commands.json ./core ./log.pvs ./pvsreport ./pc-djgpp ./pc.err
	@set -x; $(RM) -r ./core-*
	@set -x; $(RM) -r ./pc.c.out.*

################################################################################

lint:
	@./.lint.sh

################################################################################

pc-djgpp.exe:
	$(RM) ./pc-djgpp ./pc-djgpp.exe
	env PATH="$(DJGPP_DIR)/$(DJGPP_ARCH)/bin:$(DJGPP_DIR)/bin:$${PATH:-}" \
	$(DJGPP_DIR)/bin/$(DJGPP_ARCH)-gcc -march=i386 -Os $(EXTRA_CFLAGS) -o ./pc-djgpp.exe ./pc.c $(EXTRA_LDFLAGS)
	$(DJGPP_DIR)/$(DJGPP_ARCH)/bin/strip ./pc-djgpp.exe
	test -f $(CWSDSTUB) && { \
		$(DJGPP_DIR)/$(DJGPP_ARCH)/bin/exe2coff ./pc-djgpp.exe && \
		$(RM) ./pc-djgpp.exe && \
		cat $(CWSDSTUB) ./pc-djgpp > ./pc-djgpp.exe && \
		$(RM) ./pc-djgpp; }

djgpp: pc-djgpp.exe

################################################################################

pc.tos:
	env PATH="$(CROSSMINT_DIR)/$(CROSSMINT_ARCH)/bin:$(CROSSMINT_DIR)/bin:$${PATH:-}" \
	$(CROSSMINT_DIR)/bin/$(CROSSMINT_ARCH)-gcc -Os $(EXTRA_CFLAGS) -o ./pc.tos ./pc.c -lgem $(EXTRA_LDFLAGS)
	$(CROSSMINT_DIR)/$(CROSSMINT_ARCH)/bin/strip ./pc.tos

atari: pc.tos

###############################################################################

pc-elks:
	env PATH="$(IA16-GCC_DIR)/$(IA16-GCC_ARCH)/bin:$(IA16-GCC_DIR)/bin:$${PATH:-}" \
	$(IA16-GCC_DIR)/bin/$(IA16-GCC_ARCH)-gcc -march=i8086 -mtune=i8086 -melks -mregparmcall -Os $(EXTRA_CFLAGS) -o ./pc-elks ./pc.c $(EXTRA_LDFLAGS)

elks: pc-elks

################################################################################

pc-dosg.exe:
	env PATH="$(IA16-GCC_DIR)/$(IA16-GCC_ARCH)/bin:$(IA16-GCC_DIR)/bin:$${PATH:-}" \
	$(IA16-GCC_DIR)/bin/$(IA16-GCC_ARCH)-gcc -march=i8086 -mtune=i8086 -mcmodel=small -mregparmcall -Os $(EXTRA_CFLAGS) -o ./pc-dosg.exe ./pc.c $(EXTRA_LDFLAGS)

gcc-dosexe: pc-dosg.exe

################################################################################

pc-dosw.exe:
	export PATH="$(WATCOM_DIR)/binl64:$${PATH:-}" && \
	export WATCOM="$(WATCOM_DIR)" && \
	export INCLUDE="$(WATCOM_DIR)/h" && \
	$(WATCOM_DIR)/binl64/wcl -bcl=DOS -0 -mt -fpi -j -d0 -os -s -fo=pc-dosw.obj -fe=pc-dosw.exe pc.c && \
	$(RM) ./pc-dosw.obj

watcom-dos: pc-dosw.exe

################################################################################

pc-dosw.com:
	export PATH="$(WATCOM_DIR)/binl64:$${PATH:-}" && \
	export WATCOM="$(WATCOM_DIR)" && \
	export INCLUDE="$(WATCOM_DIR)/h" && \
	$(WATCOM_DIR)/binl64/owcc -c -bt=dos -bcom -march=i86 -fsigned-char -mcmodel=t -g0 -frerun-optimizer -Os -fno-stack-check -o ./pc-doswc.obj ./pc.c && \
	$(WATCOM_DIR)/binl64/owcc -bcom -s -o ./pc-dosw.com ./pc-doswc.obj && \
	$(RM) ./pc-doswc.obj

watcom-doscom: pc-dosw.com

################################################################################

pc-amiga:
	env PATH="$(AMIGA_DIR)/$(AMIGA_ARCH)/bin:$(AMIGA_DIR)/bin:$${PATH:-}" \
	$(AMIGA_DIR)/bin/$(AMIGA_ARCH)-gcc -Os $(EXTRA_CFLAGS) -o ./pc-amiga ./pc.c -noixemul $(EXTRA_LDFLAGS)
	$(AMIGA_DIR)/$(AMIGA_ARCH)/bin/strip ./pc-amiga

amiga: pc-amiga

################################################################################

# To build for Retro68, you need to clone https://github.com/johnsonjh/dpsprintf
# in the top level of the source tree - I don't want to add git submodule to the
# project and complicate things just for this.  We build our extra header files
# and the Macintosh resource files here so we don't clutter the source tree up.

# The absolute minimum memory required to launch pc on System 7.5.5 seems to
# 802K at this time - anything less and it won't open.  So we'll set the the
# # minimum required memory to 832K (851968), should leave about ~30K free.

# It will likely be necessary to periodically tweak these numbers for different
# versions of the compiler and libraries or if any build flags are modified!

pc-mac68k pc-mac68k.dsk: ./dpsprintf/dpsprintf.c ./dpsprintf/dpsprintf.h ./pc.c
	$(RM) ./dpsprintf.o ./rez.r ./extra.h ./pc-mac68k.o ./pc-mac68k.bin ./pc-mac68k.dsk 2> /dev/null
	printf '%s\n' "long double ldexpl (long double, int);" > ./extra.h
	printf '%s\n' "#include \"Processes.r\"" "#include \"Retro68.r\"" \
			"resource 'SIZE' (0) {" \
			"  reserved," \
			"  ignoreSuspendResumeEvents," \
			"  reserved," \
			"  cannotBackground," \
			"  needsActivateOnFGSwitch," \
			"  backgroundAndForeground," \
			"  dontGetFrontClicks," \
			"  ignoreChildDiedEvents," \
			"  is32BitCompatible," \
			"  notHighLevelEventAware," \
			"  onlyLocalHLEvents," \
			"  notStationeryAware," \
			"  dontUseTextEditServices," \
			"  reserved," \
			"  reserved," \
			"  reserved," \
			"  851968," \
			"  851968" \
			"};" > ./rez.r
	env PATH="$(RETRO68_DIR)/$(RETRO68_68K_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_68K_ARCH)-gcc -Oz -fdata-sections -ffunction-sections $(EXTRA_CFLAGS) -I./dpsprintf -c -o ./dpsprintf.o ./dpsprintf/dpsprintf.c -include math.h -include ./extra.h
	env PATH="$(RETRO68_DIR)/$(RETRO68_68K_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_68K_ARCH)-gcc -Oz -fdata-sections -ffunction-sections $(EXTRA_CFLAGS) -I./dpsprintf -c -o ./pc-mac68k.o ./pc.c -include ./dpsprintf/dpsprintf.h
	env PATH="$(RETRO68_DIR)/$(RETRO68_68K_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_68K_ARCH)-g++ -Wl,--gc-sections ./dpsprintf.o ./pc-mac68k.o -o pc-mac68k.bin -lRetroConsole -lm $(EXTRA_LDFLAGS)
	env PATH="$(RETRO68_DIR)/$(RETRO68_68K_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_REZ) -I$(RETRO68_DIR)/$(RETRO68_68K_ARCH)/RIncludes --copy "pc-mac68k.bin" $(RETRO68_DIR)/$(RETRO68_68K_ARCH)/RIncludes/Retro68APPL.r ./rez.r -t "APPL" -c "????" --cc pc-mac68k.dsk
	$(RM) -r ./.finf/ ./.rsrc/ ./rez.output.rsrc

mac68k: pc-mac68k

################################################################################

# For the PowerPC version, we'll ask for 1.5MB of memory.

pc-macppc pc-macppc.dsk: ./dpsprintf/dpsprintf.c ./dpsprintf/dpsprintf.h ./pc.c
	$(RM) ./dpsprintf.o ./rez.r ./extra.h ./pc-macppc.o ./pc-macppc.bin ./pc-macppc.dsk ./pc-macppc.pef 2> /dev/null
	printf '%s\n' "long double ldexpl (long double, int);" > ./extra.h
	printf '%s\n' "#include \"Processes.r\"" "#include \"Retro68.r\"" \
			"resource 'SIZE' (0) {" \
			"  reserved," \
			"  ignoreSuspendResumeEvents," \
			"  reserved," \
			"  cannotBackground," \
			"  needsActivateOnFGSwitch," \
			"  backgroundAndForeground," \
			"  dontGetFrontClicks," \
			"  ignoreChildDiedEvents," \
			"  is32BitCompatible," \
			"  notHighLevelEventAware," \
			"  onlyLocalHLEvents," \
			"  notStationeryAware," \
			"  dontUseTextEditServices," \
			"  reserved," \
			"  reserved," \
			"  reserved," \
			"  1572864," \
			"  1572864" \
			"};" > ./rez.r
	env PATH="$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_PPC_ARCH)-gcc -Oz -fdata-sections -ffunction-sections -Wno-overflow $(EXTRA_CFLAGS) -I./dpsprintf -c -o ./dpsprintf.o ./dpsprintf/dpsprintf.c -include math.h -include ./extra.h
	env PATH="$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_PPC_ARCH)-gcc -Oz -fdata-sections -ffunction-sections $(EXTRA_CFLAGS) -I./dpsprintf -c -o ./pc-macppc.o ./pc.c -include ./dpsprintf/dpsprintf.h
	env PATH="$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_PPC_ARCH)-g++ -s -Wl,--gc-sections ./dpsprintf.o ./pc-macppc.o -o pc-macppc.bin -lRetroConsole -lm $(EXTRA_LDFLAGS)
	env PATH="$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_MAKEPEF) ./pc-macppc.bin -o pc-macppc.pef
	env PATH="$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/bin:$(RETRO68_DIR)/bin:$${PATH:-}" \
	$(RETRO68_DIR)/bin/$(RETRO68_REZ) -I$(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/RIncludes --copy "pc-macppc.pef" $(RETRO68_DIR)/$(RETRO68_PPC_ARCH)/RIncludes/Retro68APPL.r ./rez.r -t "APPL" -c "????" --cc pc-macppc.dsk
	$(RM) -r ./.finf/ ./.rsrc/ ./rez.output.rsrc

macppc: pc-macppc

################################################################################

everything: pc djgpp atari elks watcom-dos watcom-doscom gcc-dosexe mac68k macppc amiga

################################################################################

.PHONY: all clean distclean lint djgpp atari elks watcom-dos watcom-doscom gcc-dosexe mac68k pc-mac68k macppc pc-macppc everything test

################################################################################
