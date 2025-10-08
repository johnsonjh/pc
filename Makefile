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

RM=rm -f
XCC=$$(command -v gcc 2> /dev/null || command -v clang 2> /dev/null || command -v c99 2> /dev/null || printf '%s\n' cc)

pc: pc.c
	@XCC="$(XCC)"; CFLAGS="$(CFLAGS)"; case "$$(uname -s 2> /dev/null || :)" in AIX) OM="OBJECT_MODE=64"; ;; esac; test "$$(command -v "$$CC" 2> /dev/null)" && { XCC="$$CC"; }; test "$$OM" && case "$$XCC" in *gcc*) CFLAGS="$${CFLAGS:-} -maix64"; ;; esac; set -x; env $${OM:-} $${XCC:?} $${CFLAGS:-} $(LDFLAGS) pc.c -o pc

all: pc

clean:
	@set -x; $(RM) ./pc

distclean: clean
	@set -x; $(RM) -r ./a.out ./ch.c ./compile_commands.json ./core ./log.pvs ./pvsreport
	@set -x; $(RM) -r ./pc.c.out.*

lint:
	@./.lint.sh

.PHONY: all clean distclean lint test
