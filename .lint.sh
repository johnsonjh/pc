#!/usr/bin/env sh
# shellcheck disable=SC2248

C_SRC="./pc.c"
S_SRC="./.lint.sh"

ORSTLINT="$(command -v /opt/developerstudio12.6/bin/lint 2> /dev/null || \
            command -v /opt/oracle/developerstudio12.6/bin/lint 2> /dev/null || \
            printf '%s\n' true)"
CPPCHECK="$(command -v cppcheck 2> /dev/null || printf '%s\n' true)"
SHELLCHECK="$(command -v shellcheck 2> /dev/null || printf '%s\n' true)"

test "${SHELLCHECK:?}" != "true" && printf '%s\n' "ShellCheck..."
"${SHELLCHECK:?}" -o any,all ${S_SRC:?}

test "${ORSTLINT:?}" != "true" && printf '%s\n' "Oracle Lint..."
"${ORSTLINT:?}" -std=c99 -err=warn -errtags=yes -errfmt=src \
                -errchk=structarg,longptr64,parentheses,locfmtchk \
                -errsecurity=extended -errshort=full -fd \
                -erroff=E_SEC_STRNCPY_WARN,E_SEC_RAND_WARN ${C_SRC:?}

test "${CPPCHECK:?}" != "true" && printf '%s\n' "Cppcheck..."
"${CPPCHECK:?}" --inline-suppr --force --check-level=exhaustive \
                -DFREE=free -DHAS_INCLUDE=0 -D_DARWIN_C_SOURCE -D_GNU_SOURCE \
                -D_NETBSD_SOURCE -D_OPENBSD_SOURCE -D_POSIX_C_SOURCE=200809L \
                -D__BSD_VISIBLE=1 -UPAGESIZE -UPAGE_SIZE -U_PC_FILESIZEBITS \
                -D__EXTENSIONS__ --quiet ${C_SRC:?}
