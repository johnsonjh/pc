<!-- vim: set nocp expandtab ft=markdown : -->
<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2025 Jeffrey H. Johnson -->
<!-- scspell-id: 498fb4ee-a43d-11f0-aae1-80ee73e9b8e7 -->
# `pc`: programmers calculator

* `pc` is a highly portable and greatly enhanced fork of the venerable
  (*SGI*→*Be*→*Haiku*) `pc` programmers calculator—the ultimate
  integer calculator for C programmers, supporting casts, modulo, shifts,
  bitwise operations, and more, in exactly the way C programmers expect.

## Portability

* Runs on nearly every **POSIX**-*ish* system with a *C99* compiler.
* Currently developed on IBM **AIX**, and regularly tested on
  **PASE for IBM i** (OS/400), **macOS**, **FreeBSD**, **NetBSD**,
  **OpenBSD**, **Solaris**, **illumos**, **Haiku**, and
  **Windows** (*MinGW-w64*).

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

## Usage

* Supports **interactive**, **command-line** (`pc 1+1`), and **piped** usage
* **Examples:**
  * `0xFF & 0x0F`
  * `1 << 12`
  * `a = 0x1234; a & 0xFF`
  * `. + 4` (add to previous result)
  * Semicolon suppresses regular output
