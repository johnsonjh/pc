#!/usr/bin/env sh
# shellcheck disable=SC2248

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
# scspell-id: a91f10f2-a39e-11f0-a871-80ee73e9b8e7

set -eu

CH="$(command -v ch 2> /dev/null || printf '%s\n' true)"
test "${CH:?}" != "true" \
  && {
    printf '%s\n' "Ch..."
    sed '1,3d' ./pc.c > ./ch.c
    ch ./ch.c "1+1" > /dev/null
    rm -f ./ch.c > /dev/null 2>&1
  }

SHELLCHECK="$(command -v shellcheck 2> /dev/null || printf '%s\n' true)"
test "${SHELLCHECK:?}" != "true" && printf '%s\n' "ShellCheck..."
"${SHELLCHECK:?}" -o any,all ./.lint.sh

ORSTLINT="$(command -v /opt/developerstudio12.6/bin/lint 2> /dev/null \
  || command -v /opt/oracle/developerstudio12.6/bin/lint 2> /dev/null \
  || printf '%s\n' true)"
test "${ORSTLINT:?}" != "true" && printf '%s\n' "Oracle Lint..."
"${ORSTLINT:?}" -std=c99 -err=warn -errtags=yes -errfmt=src \
  -errchk=structarg,longptr64,parentheses,locfmtchk \
  -errsecurity=extended -errshort=full -fd \
  -erroff=E_SEC_STRNCPY_WARN,E_SEC_RAND_WARN pc.c

CPPCHECK="$(command -v cppcheck 2> /dev/null || printf '%s\n' true)"
test "${CPPCHECK:?}" != "true" && printf '%s\n' "Cppcheck..."
"${CPPCHECK:?}" --inline-suppr --force --check-level=exhaustive \
  -DFREE=free -DHAS_INCLUDE=0 -D_DARWIN_C_SOURCE -D_GNU_SOURCE \
  -D_NETBSD_SOURCE -D_OPENBSD_SOURCE -D_POSIX_C_SOURCE=200809L \
  -D__BSD_VISIBLE=1 -UPAGESIZE -UPAGE_SIZE -U_PC_FILESIZEBITS \
  -D__EXTENSIONS__ --quiet pc.c

REUSE="$(command -v reuse 2> /dev/null || printf '%s\n' true)"
test "${REUSE:?}" != "true" && printf '%s\n' "REUSE..."
"${REUSE:?}" lint -q || "${REUSE:?}" lint
