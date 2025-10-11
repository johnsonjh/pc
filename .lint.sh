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

# shellcheck disable=SC2015,SC3040
(set -o pipefail || :) > /dev/null 2>&1 && set -o pipefail > /dev/null 2>&1 || :

# Setup

export WITH_TERNARY=1
export WITH_BASE36=1

xline()
{
  cols=$(tput cols 2> /dev/null || printf '%s\n' 80)

  if [ "${cols:?}" -gt 999 ] || [ "${cols:?}" -lt 10 ]; then
    cols=10
  fi

  line=
  i=0

  while [ "${i:?}" -lt "${cols:?}" ]; do
    line="${line:-}="
    i=$((i + 1))
  done

  printf '%s\n' "${line:?}"
}

set -eu

# Cleanup
"${MAKE:-make}" distclean > /dev/null 2>&1

# Codespell
CODESPELL="$(command -v codespell 2> /dev/null || printf '%s\n' true)"
test "${CODESPELL:?}" != "true" && {
  xline
  printf '%s\n' "Codespell..."
}
"${CODESPELL:?}" -L PASE .

# REUSE
REUSE="$(command -v reuse 2> /dev/null || printf '%s\n' true)"
test "${REUSE:?}" != "true" && {
  xline
  printf '%s\n' "REUSE..."
}
"${REUSE:?}" lint -q || "${REUSE:?}" lint

# SoftIntegration Ch C/C++ Interpreter
CH="$(command -v ch 2> /dev/null || printf '%s\n' true)"
test "${CH:?}" != "true" \
  && {
    xline
    printf '%s\n' "Ch..."
    sed '1,3d' ./pc.c > ./ch.c
    ch ./ch.c "1+1" > /dev/null
    rm -f ./ch.c > /dev/null 2>&1
  }

# Cppi
CPPI="$(command -v cppi 2> /dev/null || printf '%s\n' true)"
test "${CPPI:?}" != "true" && {
  xline
  printf '%s\n' "Cppi..."
}
"${CPPI:?}" -ac ./pc.c

# ShellCheck
SHELLCHECK="$(command -v shellcheck 2> /dev/null || printf '%s\n' true)"
test "${SHELLCHECK:?}" != "true" && {
  xline
  printf '%s\n' "ShellCheck..."
}
"${SHELLCHECK:?}" -o any,all ./.lint.sh

# Oracle Lint
ORSTLINT="$(command -v /opt/developerstudio12.6/bin/lint 2> /dev/null \
  || command -v /opt/oracle/developerstudio12.6/bin/lint 2> /dev/null \
  || printf '%s\n' true)"
test "${ORSTLINT:?}" != "true" && {
  xline
  printf '%s\n' "Oracle Lint..."
}
"${ORSTLINT:?}" -std=c99 -err=warn -errtags=yes -errfmt=src \
  -errchk=structarg,longptr64,parentheses,locfmtchk \
  -errsecurity=extended -errshort=full -fd \
  -DWITH_TERNARY=1 -DWITH_BASE36=1 \
  -erroff=E_SEC_STRNCPY_WARN,E_SEC_RAND_WARN pc.c

# NetBSD Lint
case "$(uname -s 2> /dev/null || :)" in
NetBSD)
  {
    LINT="$(command -v /usr/bin/lint 2> /dev/null || printf '%s\n' true)"
    test "${LINT:?}" != "true" && {
      xline
      printf '%s\n' "NetBSD Lint..."
    }
    lint -DWITH_TERNARY=1 -DWITH_BASE36=1 -S -a -aa -b -c -e -g -h -P -r -u -z pc.c 2>&1 \
      | grep -Ev '(^lint: cannot find llib-lc\.ln$|^pc\.c:$)' || :
  }
  ;;
*) : ;;
esac

# PVS-Studio
command -v bear > /dev/null 2>&1 && {
  PVSSTUDIO="$(command -v pvs-studio-analyzer || printf '%s\n' true)"
  test "${PVSSTUDIO:?}" != "true" && {
    xline
    printf '%s\n' "PVS-Studio..."
    "${MAKE:-make}" clean > /dev/null 2>&1
    rm -rf ./compile_commands.json ./log.pvs ./pvsreport
    bear -- "${MAKE:-make}"
    pvs-studio-analyzer analyze --intermodular \
      -j "$(nproc || printf '%s\n' '1' || true)" -o log.pvs
    plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -o pvsreport
    "${MAKE:-make}" clean > /dev/null 2>&1
  }
}

# GCC Analyzer
case "$(uname -s 2> /dev/null || :)" in
Darwin) : ;; # GCC might not be real GCC on Darwin
*) {
  GCC="$(command -v gcc || printf '%s\n' true)"
  test "${GCC:?}" != "true" && {
    xline
    printf '%s\n' "GCC Analyzer..."
    "${GCC:?}" -fanalyzer -DWITH_TERNARY=1 -DWITH_BASE36=1 -std=c99 -Wall -Wextra -Wpedantic -Werror -O3 pc.c -o pc.gcc
    rm -f ./pc.gcc > /dev/null 2>&1
  }
} ;;
esac

# Clang Analyzer
SCANBUILD="$(command -v scan-build 2> /dev/null || printf '%s\n' true)"
test "${SCANBUILD:?}" != "true" && {
  xline
  printf '%s\n' "Clang Analyzer..."
  "${SCANBUILD:?}" --status-bugs -maxloop 8 "${MAKE:-make}"
  "${MAKE:-make}" clean > /dev/null 2>&1
}

# Cppcheck
CPPCHECK="$(command -v cppcheck 2> /dev/null || printf '%s\n' true)"
test "${CPPCHECK:?}" != "true" && {
  xline
  printf '%s\n' "Cppcheck..."
}
"${CPPCHECK:?}" --inline-suppr --force --check-level=exhaustive \
  -DFREE=free -DHAS_INCLUDE=0 -D_DARWIN_C_SOURCE -D_GNU_SOURCE \
  -D_NETBSD_SOURCE -D_OPENBSD_SOURCE -D_POSIX_C_SOURCE=200809L \
  -D__BSD_VISIBLE=1 -UPAGESIZE -UPAGE_SIZE -U_PC_FILESIZEBITS \
  -D__EXTENSIONS__ -DWITH_TERNARY=1 -DWITH_BASE36=1 --quiet pc.c

# Clang -Weverything:
# clang -DWITH_TERNARY=1 -DWITH_BASE36=1 -Weverything -Wno-unsafe-buffer-usage -Wno-unused-macros -Wno-reserved-macro-identifier -Wno-date-time pc.c -o pc
# clang -m32 -DWITH_TERNARY=1 -DWITH_BASE36=1 -Weverything -Wno-unsafe-buffer-usage -Wno-unused-macros -Wno-reserved-macro-identifier -Wno-date-time pc.c -o pc
# clang -m32 -DWITHOUT_LONG_LONG -DWITH_TERNARY=1 -DWITH_BASE36=1 -Weverything -Wno-unsafe-buffer-usage -Wno-unused-macros -Wno-reserved-macro-identifier -Wno-date-time -Wno-shift-count-overflow pc.c -o pc

# IA16-GCC COM:
# env PATH=$HOME/src/build-ia16/prefix/ia16-elf/bin:$HOME/src/build-ia16/prefix/bin $HOME/src/build-ia16/prefix/bin/ia16-elf-gcc pc.c -o pc.com -DWITHOUT_LOCALE=1 -march=i8086 -mregparmcall -Os -mcmodel=small -Wall -Wextra -Wpedantic -std=c99
# IA16-GCC EXE:
# env PATH=$HOME/src/build-ia16/prefix/ia16-elf/bin:$HOME/src/build-ia16/prefix/bin $HOME/src/build-ia16/prefix/bin/ia16-elf-gcc pc.c -o pc.exe -DWITHOUT_LOCALE=1 -march=i8086 -mregparmcall -Os -mcmodel=medium -Wall -Wextra -Wpedantic -std=c99
# IA16-GCC ELKS:
# env PATH=$HOME/src/build-ia16/prefix/ia16-elf/bin:$HOME/src/build-ia16/prefix/bin $HOME/src/build-ia16/prefix/bin/ia16-elf-gcc pc.c -o pc -melks -Os -mtune=i8086 -Wall -Wextra -Wpedantic -std=c99 -D__far= -mregparmcall

# OWC2 DOS EXE:
# env INCLUDE=/opt/watcom/h /opt/watcom/binl64/wcl -bcl=DOS -0 -ml -fpi -zp2 -j -s pc.c

# Final xline
xline
