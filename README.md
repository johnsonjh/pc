<!-- vim: set nocp expandtab ft=markdown cc=80 : -->
<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2025 Jeffrey H. Johnson -->
<!-- scspell-id: 498fb4ee-a43d-11f0-aae1-80ee73e9b8e7 -->
# `pc`: programmers calculator

`pc` is a substantially enhanced fork of the SGI/Be/Haiku programmers calculator.

## Syntax and operators

* **Automatic input and output base conversions:**:
  * Binary (base 2): `0b111101001101001`
  * Octal (base 8): `0o75151` (or `0075151`)
  * Decimal (base 10): `31337`
  * Hex (base 16): `0x7a69`
  * Roman: `0rMCCCXXXVII`
  * ASCII: `'zi'`
* **Optional support for:**
  * Ternary (base 3): `0t1120222122`
  * Base 36: `0zO6H`
[]()

[]()
* **Operators:**
  * `++`, `--`, `~`, `!`, `*`, `/`, `%`, `+`, `-`, `<<`, `>>`, `<`, `>`,
    `<=`, `>=`, `==`, `!=`, `&`, `^`, `|`, `&&`, `||`
[]()

[]()
* **Precedence (highest to lowest):**
  * `++`, `--`, `~`, `!`
  * `*`, `/`, `%`
  * `+`, `-`
  * `<<`, `>>`
  * `<`, `>`, `<=`, `>=`
  * `==`, `!=`
  * `&`
  * `^`
  * `|`
  * `&&`
  * `||`
[]()

[]()
* **Assignment operators:**
  * `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `^=`, `|=`, `<<=`, `>>=`
[]()

[]()
* **Negation:**
  * `!` is *logical*, not *bitwise* (use `~` for bitwise negation)
[]()

[]()
* **Parentheses:** Full support for grouping and nesting.
[]()

[]()
* **Explicit modes:**
  * Three calculation modes are available, via named commands:
    * **`auto`**, **`signed`**, and **`unsigned`**.  The `mode` command
      can be used to query the current calculation mode.
    * **All operations** ***except comparisons*** are performed using
      ***unsigned integer*** calculations in the default **`auto`** mode.
  * Braces (*e.g.*, `{ … }`) force **signed** calculation (*regardless of
    global calculation mode*).
  * Brackets (*e.g.*, `[ … ]` force **unsigned** calculation.

## User variables, builtins, and registers

* **Variables:**
  * All C-style names (starting with alpha or `_`, followed by any
    alphanumeric or `_`) can be used.
  * User variables are created on first use
    (and automatically initialized to zero), and can be listed with `vars`.
  * **Examples:**
    * `x = 5`
    * `x = y = 10`
    * `x = (y++ + 5) * 2`
    * `(y * 2) + (x & 0xffeef)`
    * `. << 2`
    * `z = .`
[]()

[]()
* **Builtins:**
  * Builtins provide access to about **50** system constants and runtime
    values, which can be listed with the `help` command.
[]()

[]()
* **Registers:**
  * Perform time formatting or casting by storing to special purpose registers
    (*i.e.*, *unsigned* C types `char`, `short`, `int`, `long`, `long long`),
    list via the `regs` command).
    * Stores to registers clamp and mask to the register width.

## Portability

* Easily portable to any **POSIX**-*ish* system with a *C99* compiler
  (or [Ch](https://www.softintegration.com/) 8.5), less easily to others.
* Currently developed on IBM **AIX**, and regularly tested on
  **PASE for IBM i** (OS/400), **macOS**, **FreeBSD**, **NetBSD**,
  **OpenBSD**, **Solaris**, **illumos**, **Haiku**, **Windows** (*MinGW-w64*),
  **Atari ST** (**TOS** and **MINT**, *built using*
  [CrossMiNT](https://tho-otto.de/crossmint.php)), **Amiga 68K** (*built using*
  [Amiga-GCC](https://franke.ms/git/bebbo/amiga-gcc)),
  [**ELKS**](https://github.com/ghaerr/elks) (*built using*
  [GCC-IA16](https://gitlab.com/tkchia/build-ia16)),
  and **DOS** (*built using* [DJGPP](https://www.delorie.com/djgpp/),
  [GCC-IA16](https://gitlab.com/tkchia/build-ia16), *and*
  [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2)).

## Build

* You can try out `pc` by **executing [`./pc.c`](pc.c) directly**,
  and it should *just run* (using *shell* *magic*), or,
* Build it with `make`.  Standard environment variables (*e.g.*, `CC`,
  `CFLAGS`, `LDFLAGS`) are respected.
* Common line editing packages (`libedit`, `editline`, `readline`, and
  `linenoise`) are supported and usually automatically configured (via
  *`pkg-config`* *magic*).
* Review the [`Makefile`](Makefile) to see available build options.

## Usage

* Supports **interactive**, **command-line** (`pc 1+1`), **file** (`take`
  command), and **piped**/**redirected** usage.
[]()

[]()
* **Interactive examples:**
  * `0xFF & 0x0F`
  * `1 << 12`
  * `a = 0x1234; a & 0xFF`
  * `. + 4` (add to previous result)
  * `take examples/sqrt.pc` (reads from `examples/sqrt.pc`)
[]()

[]()
* [`easter.pc`](examples/easter.pc) is a program that calculates the date of
  Easter (and checks if it's today).
  * You can modify the program or use something like
    [`faketime`](https://github.com/wolfcw/libfaketime)
    (*i.e.*, `faketime -m '2025-04-20 12:00:00'`) if you want to test
    arbitrary dates.  You can see the
    [full output of this program here](examples/easter.txt) (with all optional
    base conversions enabled).
[]()

[]()
* [`sqrt.pc`](examples/sqrt.pc) is a program that calculates the square root
  of `n` (where `n` is, by default, a large random number).  You can see the
  [full output of this program here](examples/sqrt.txt).
[]()

[]()
* [`weekday.pc`](examples/weekday.pc) is a program that calculates the day of
  the week.  The formatted date is also displayed (via a store to the `GT`
  register) for confirmation.  You can see the
  [full output of this program here](examples/weekday.txt).

## Testing

* **Linting**:
  * C code must pass [Cppcheck](https://cppcheck.sourceforge.io/),
    [Clang Analyzer](https://clang-analyzer.llvm.org/),
    [GCC Analyzer](https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html),
    [**PVS-Studio**](https://pvs-studio.com/),
    [NetBSD Lint](https://man.netbsd.org/lint.1),
    [Oracle Lint](https://www.oracle.com/application-development/developerstudio/),
    [Flawfinder](https://dwheeler.com/flawfinder/) (*`-m3`*),
    [Cppi](https://www.gnu.org/software/cppi/),
    [Ch](https://www.softintegration.com/), and
    [`clang -Weverything`](.lint.sh) linting and static analysis.
  * All shell code in the repository must pass
    [ShellCheck](https://www.shellcheck.net/), and the
    [`Makefile`](Makefile) is compatible with **POSIX**, **AIX**, **BSD**,
    and **GNU** style `make` tools.

## License

* `pc` is distributed under the terms of the [**MIT License**](LICENSE).
* The repository is compliant with version 3.3 of the
  [REUSE](https://reuse.software/) specification.
