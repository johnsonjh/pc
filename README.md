<!-- vim: set nocp expandtab ft=markdown : -->
<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2025 Jeffrey H. Johnson -->
<!-- scspell-id: 498fb4ee-a43d-11f0-aae1-80ee73e9b8e7 -->
# `pc`: programmers calculator

* `pc` is a greatly enhanced fork of the venerable (*SGI*→*Be*→*Haiku*)
  `pc` programmers calculator—the ultimate integer calculator for C
  programmers, supporting casts, modulo, shifts, bitwise operations, and
  more, in exactly the way C programmers expect on their system.

## Syntax and operators

* **Automatic input base conversion**:
  * Binary (base 2): `0b111101001101001`
  * Ternary (base 3): `0t1120222122`
  * Octal (base 8): `0075151`
  * Binary (base 10): `31337`
  * Hex (base 16): `0x7a69`
  * Base 36: `0zO6H`
  * ASCII: `'zi'`

* **Operators:**
  * `++`, `--`, `~`, `!`, `*`, `/`, `%`, `+`, `-`, `<<`, `>>`, `<`, `>`,
    `<=`, `>=`, `==`, `!=`, `&`, `^`, `|`, `&&`, `||`

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

* **Assignment operators:**
  * `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `^=`, `|=`, `<<=`, `>>=`
    * **Logical negation:** `!` is *logical*, not *bitwise*
      (use `~` for bitwise negation).

* **Parentheses:** Full support for grouping and nesting.

* **Diagnostics:** Warnings for **overflow**, **underflow**,
  **bad shifts**, **division**/**modulo** **by** **zero**, etc.

## User variables, builtins, and registers

* **Variables:**
  * All C-style names (starting with alpha or `_`, followed by any
    alphanumeric or `_`) can be used.
  * User variables are created on first use
    (and automatically initialized to zero), and can be listed with `vars`.
  * **Examples:**
    * `x = 5`
    * `x = y = 10`
    * `x = (y + 5) * 2`
    * `(y * 2) + (x & 0xffeef)`
    * 
      ```
      x = 5
                         5  0x00000005
      x = y = 10
                        10  0x0000000a
      x = (y + 5) * 2
                        30  0x0000001e
      (y * 2) + (x & 0xffeef)
                        34  0x00000022          char: ......."
      vars
      User variables:
        x                =                   30  0x0000001e
        y                =                   10  0x0000000a
      x=y=
      Variable 'y' unset
      Variable 'x' unset
      ```

* **Builtins:**
  * Builtins provide access to **50** or more system constants and runtime
    values, which can be listed with `help`.

* **Registers:**
  * Perform casts by storing into explicitly sized registers
    (*i.e.*, *unsigned* C types `char`, `short`, `int`, `long`, `long long`),
    that can be listed with `regs`.
    * Stores to registers clamp and mask to register width
    * Loads from registers reflect the masked value
  * **Example:**
    ```
    GLL=GL=GI=GS=GC=-1;
    regs
    Registers:
    GC               =                  255  0x000000ff
    GS               =                65535  0x0000ffff
    GI               =           4294967295  0xffffffff
    GL               = 18446744073709551615  0xffffffffffffffff  sign: -1
    GLL              = 18446744073709551615  0xffffffffffffffff  sign: -1
    ```

## Portability

* Portable to any **POSIX**-*ish* system with a *C99* compiler.
* Currently developed on IBM **AIX**, and regularly tested on
  **PASE for IBM i** (OS/400), **macOS**, **FreeBSD**, **NetBSD**,
  **OpenBSD**, **Solaris**, **illumos**, **Haiku**, and
  **Windows** (*MinGW-w64*).

### Build

* You can try it out `pc` by **executing [`./pc.c`](pc.c) directly**,
  and it will *just run* (through the magic of polyglot programming), or,
* Build it with `make`.  Standard environment variables  (*e.g.*, `CC`,
  `CFLAGS`, `LDFLAGS`) are respected.

## Usage

* Supports **interactive**, **command-line** (`pc 1+1`), and **piped** usage
* **Examples:**
  * `0xFF & 0x0F`
  * `1 << 12`
  * `a = 0x1234; a & 0xFF`
  * `. + 4` (add to previous result)
  * Semicolon suppresses regular output

## Testing

* **Linting**:
  * C code must pass [Cppcheck](https://cppcheck.sourceforge.io/), [Clang Analyzer](https://clang-analyzer.llvm.org/), [GCC Analyzer](https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html),
    **PVS-Studio**, [NetBSD Lint](https://man.netbsd.org/lint.1), [Oracle Lint](https://www.oracle.com/application-development/developerstudio/), [Flawfinder](https://dwheeler.com/flawfinder/) (*`-m3`*),
    [Cppi](https://www.gnu.org/software/cppi/), [Ch](https://www.softintegration.com/), and [`clang -Weverything`](.lint.sh) linting and static analysis.
  * All shell code in the repository must pass [ShellCheck](https://www.shellcheck.net/), and the
    `Makefile` is compatible with `POSIX`, `AIX`, `BSD`, and `GNU` style
    `make` tools.

## License

* `pc` is distributed under the terms of the [**MIT License**](LICENSE).
* The repository is complaint with version 3.3 of the [REUSE](https://reuse.software/) specification.
