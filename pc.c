#/* \
PID=$$; p=$0; rlwrap="$(command -v rlwrap 2> /dev/null || :)"; cc="$( command -v gcc 2>/dev/null || command -v clang 2>/dev/null || command -v c99 2>/dev/null || command -v ibm-clang 2>/dev/null || :)"; case "$(uname -s 2>/dev/null || :)" in AIX) export OBJECT_MODE=64; case "${cc:-cc}" in *gcc*) CFLAGS="${CFLAGS:-} -maix64" ;; esac ;; esac; if "${cc:-cc}" ${CFLAGS:-} ${LDFLAGS-} -o "${p:?}.out.${PID:?}" "${p:?}"; then case "${p:?}" in *"/"*) dir=${0%"/"*} ;; *) dir=. ;; esac; PATH="${dir:?}:${PATH:-.}"; "${rlwrap:-env}" "${p:?}.out.${PID:?}" "$@"; rm -f "${p:?}.out.${PID:?}" > /dev/null 2>&1; exit 0;fi;exit 1
#*/ /* Remove from this line to the top of the file for SoftIntegration Ch. */

/*
 * pc2: programmer's calculator
 * SPDX-License-Identifier: MIT
 * scspell-id: db65bb93-4b7b-11ed-bd13-80ee73e9b8e7
 */

/*
 * Copyright (c) 1993 Dominic Giampaolo <dbg@be.com>
 * Copyright (c) 1994 Joel Tesler <joel@engr.sgi.com>
 * Copyright (c) 2005 Axel Dörfler <axeld@pinc-software.de>
 * Copyright (c) 2005 Ingo Weinhold <ingo_weinhold@gmx.de>
 * Copyright (c) 2009 Oliver Tappe <zooey@hirschkaefer.de>
 * Copyright (c) 2017 Tuan Kiet Ho <tuankiet65@gmail.com>
 * Copyright (c) 2019 Adrien Destugues <pulkomandy@pulkomandy.tk>
 * Copyright (c) 2022-2025 Jeffrey H. Johnson <trnsz@pobox.com>
 * Copyright (c) 2022-2025 The DPS8M Development Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This program implements a simple recursive descent parser that
 * understands pretty much all standard C language math and logic
 * expressions.  It handles the usual add, subtract, multiply, divide,
 * and mod sort of stuff.  It can also deal with logical/relational
 * operations and expressions.  The logic/relational operations AND, OR,
 * NOT, and EXCLUSIVE OR, &&, ||, ==, !=, <, >, <=, and >= are all
 * supported.  It also handles parens and nested expressions as well as
 * left and right shifts.  There are variables and assignments (as well
 * as assignment operators like "*=").
 *
 * The other useful feature is that you can use "." in an expression
 * to refer to the value from the previous expression (just like bc).
 *
 * Multiple statements can be separated by semi-colons (;) on a single
 * line (though a single statement can't span multiple lines).
 *
 * This calculator is mainly a programmers calculator because it
 * doesn't work in floating point and only deals with integers.
 *
 * I wrote this because the standard unix calculator (bc) doesn't
 * offer a useful modulo, it doesn't have left and right shifts and
 * sometimes it is a pain in the ass to use (but I still use bc for
 * things that require any kind of floating point).  This program is
 * great when you have to do address calculations and bit-wise
 * masking/shifting as you do when working on kernel type code.  It's
 * also handy for doing quick conversions between decimal, hex and ascii
 * (and if you want to see octal for some reason, just put it in the
 * fprintf string).
 *
 * The parser is completely separable and could be spliced into other
 * code very easy.  The routine parse_expression() just expects a char
 * pointer and returns the value.  Implementing command line editing
 * would be easy using a readline() type of library.
 *
 * This isn't the world's best parser or anything, but it works and
 * suits my needs.  It faithfully implements C style precedence of
 * operators for:
 *
 *      ++ -- ~ ! * / % + - << >> < > <= >= == != & ^ | && ||
 *
 * (in that order, from greatest to least precedence).
 *
 * Note: The ! unary operator is a logical negation, not a bitwise
 * negation (if you want bitwise negation, use ~).
 *
 * I've been working on adding variables and assignments, and I've
 * just (10/26/94) got it working right and with code I'm not ashamed of.
 * Now you can have variables (no restrictions on length) and assign to
 * them and use them in expressions.  Variable names have the usual C
 * rules (i.e. alpha or underscore followed by alphanumeric and
 * underscore).  Variables are initialized to zero and created as needed.
 * You can have any number of variables. Here are some examples:
 *
 *      x = 5
 *      x = y = 10
 *      x = (y + 5) * 2
 *      (y * 2) + (x & 0xffeef)
 *
 * Assignment operators also work.  The allowable assignment operators
 * are (just as in C):
 *
 *      +=, -=, *=, /=, %=, &=, ^=, |=, <<=, and >>=
 *
 * The basic ideas for this code came from the book "Compiler Design
 * in C", by Allen I. Holub, but I've extended them significantly.
 *
 * If you find bugs or parsing bogosites, I'd like to know about them
 * so I can fix them.  Other comments and criticism of the code are
 * welcome as well.
 *
 * Thanks go to Joel Tesler (joel@engr.sgi.com) for adding the ability
 * to pass command line arguments and have pc evaluate them instead of reading
 * from stdin.
 *
 *      Dominic Giampaolo
 *      dbg@be.com (though this was written while I was at sgi)
 */

#define PC_SOFTWARE_NAME "pc2"
#define PC_VERSION_MAJOR 0
#define PC_VERSION_MINOR 2
#define PC_VERSION_PATCH 0
#define PC_VERSION_OSHIT 0

#if defined (STR_HELPER)
# undef STR_HELPER
#endif

#define STR_HELPER(x) #x

#if defined (STR)
# undef STR
#endif

#define STR(x) STR_HELPER(x)

#ifndef PC_SOFTWARE_DATE
# if defined (__TIMESTAMP__)
#  define PC_SOFTWARE_DATE STR(__TIMESTAMP__)
# elif defined (__DATE__)
#  define PC_SOFTWARE_DATE STR(__DATE__)
# else
#  define PC_SOFTWARE_DATE 0
# endif
#endif

#if defined (__MVS__) || defined (_AIX)
# if defined (_ALL_SOURCE)
#  undef _ALL_SOURCE
# endif
# define _ALL_SOURCE
#endif

#if defined (_POSIX_C_SOURCE)
# undef _POSIX_C_SOURCE
#endif

#if !defined (__OpenBSD__)
# define _POSIX_C_SOURCE 200809L
#endif

#if !defined (_GNU_SOURCE)
# define _GNU_SOURCE
#endif

#if !defined (_NETBSD_SOURCE)
# define _NETBSD_SOURCE
#endif

#if !defined (_OPENBSD_SOURCE)
# define _OPENBSD_SOURCE
#endif

#if !defined (_DARWIN_C_SOURCE)
# define _DARWIN_C_SOURCE
#endif

#if !defined (__BSD_VISIBLE)
# define __BSD_VISIBLE 1
#endif

#if !defined (__EXTENSIONS__)
# define __EXTENSIONS__
#endif

#if defined (HAS_INCLUDE)
# undef HAS_INCLUDE
#endif

#if defined __has_include
# define HAS_INCLUDE(inc) __has_include(inc)
#else
# define HAS_INCLUDE(inc) 0
#endif

#include <ctype.h>  /* isalnum, isalpha, isdigit, isprint, isspace ... */
#include <errno.h>  /* errno ...                                       */
#include <limits.h> /* LONG_MIN, ULONG_MAX ...                         */
#include <stddef.h> /* ptrdiff_t ...                                   */
#include <stdio.h>  /* fprintf, NULL, stderr, fgets, stdin ...         */
#include <stdlib.h> /* free, malloc, exit, abort, rand, realloc ...    */
#include <string.h> /* strncmp, strlen, strcmp, strdup, strncat ...    */
#include <time.h>   /* time ...                                        */
#include <unistd.h> /* getpid, getuid, getgid ...                      */

/* cppcheck-suppress preprocessorErrorDirective */
#if defined (__OpenBSD__) || HAS_INCLUDE (<sys/param.h>)
# include <sys/param.h> /* PAGESIZE, PAGE_SIZE ... */
#endif

#if defined (_CH_)
# if !defined (NO_LOCALE)
#  define NO_LOCALE
# endif
#endif

#if !defined (NO_LOCALE)
# include <locale.h>
# if defined (__APPLE__)
#  include <xlocale.h>
# endif
#endif

#if (defined (WITH_LIBEDIT) + defined (WITH_READLINE) + defined (WITH_LINENOISE)) > 1
# error "Only one of WITH_LIBEDIT, WITH_READLINE, or WITH_LINENOISE can be defined."
#endif

#if defined (WITH_READLINE)
# include <readline/readline.h>
# include <readline/history.h>
#endif

#if defined (WITH_LIBEDIT)
# include <editline/readline.h>
#endif

#if defined (WITH_LINENOISE)
# include <linenoise.h>
#endif

#if defined (__MVS__) && !defined (__clang_version__)
# if defined (inline)
#  undef inline
# endif
# define inline //-V1059
#endif

#define AND             '&'
#define BANG            '!'
#define COMMA           ','
#define DIVISION        '/'
#define EQUAL           '='
#define GREATER_THAN    '>'
#define LESS_THAN       '<'
#define LPAREN          '('
#define MINUS           '-'
#define MODULO          '%'
#define NEGATIVE        '-'
#define NOTHING         '\0'
#define OR              '|'
#define PLUS            '+'
#define RPAREN          ')'
#define SEMI_COLON      ';'
#define SHIFT_L         '<'
#define SHIFT_R         '>'
#define SINGLE_QUOTE    '\''
#define TIMES           '*'
#define TWIDDLE         '~'
#define USE_LAST_RESULT '.'
#define XOR             '^'

#define PFC_INT8 "%c%c%c%c%c%c%c%c"

#define PBI_8(i)                \
  (((i) & 0x80ll) ? '1' : '0'), \
  (((i) & 0x40ll) ? '1' : '0'), \
  (((i) & 0x20ll) ? '1' : '0'), \
  (((i) & 0x10ll) ? '1' : '0'), \
  (((i) & 0x08ll) ? '1' : '0'), \
  (((i) & 0x04ll) ? '1' : '0'), \
  (((i) & 0x02ll) ? '1' : '0'), \
  (((i) & 0x01ll) ? '1' : '0')

#define PFC_INT16 PFC_INT8  PFC_INT8
#define PFC_INT32 PFC_INT16 PFC_INT16
#define PFC_INT64 PFC_INT32 PFC_INT32

#define PBI_16(i) PBI_8((i)  >>  8), PBI_8(i)
#define PBI_32(i) PBI_16((i) >> 16), PBI_16(i)
#define PBI_64(i) PBI_32((i) >> 32), PBI_32(i)

#if defined (FREE)
# undef FREE
#endif

static const int never = 0;

#define FREE(p)   \
  do              \
    {             \
      free ((p)); \
      (p) = NULL; \
    }             \
  while (never)

/* Determine sane size for input buffers */

/* Always calculate our own INPUT_BUFF */
#if defined (INPUT_BUFF)
# undef INPUT_BUFF
#endif

/* Always calculate our own INPUT_BUFF_FB */
#if defined (INPUT_BUFF_FB)
# undef INPUT_BUFF_FB
#endif

/* Fallback input buffer size */
#define INPUT_BUFF_FB 256

/* Use PIPE_BUF if larger than INPUT_BUFF_FB */
#if defined (PIPE_BUF)
# if PIPE_BUF < INPUT_BUFF_FB
#  define INPUT_BUFF INPUT_BUFF_FB
# else
#  define INPUT_BUFF PIPE_BUF
# endif
#else
# define INPUT_BUFF INPUT_BUFF_FB
#endif

/*
 * Keep '# define USE_LONG_LONG' if your compiler supports the the "long long"
 * type and your fprintf supports the '%lld' format specifier.  If it doesn't
 * just comment out the #define below and pc will make due with plain longs.
 *
 * If happen to have a compiler that supports a "long long" type and fprintf
 * without "%lld', you could try using https://github.com/johnsonjh/dpsprintf
 */

#if !defined (USE_LONG_LONG)
# define USE_LONG_LONG
#endif

#if defined (USE_LONG_LONG)
# define LONG  long long
# define ULONG unsigned long long
#else
# define LONG  long
# define ULONG unsigned long
#endif

#if !defined (NO_LOCALE)
# define XSTR_EMAXLEN 32767

static const char *
xstrerror_l (int errnum)
{
  int saved = errno;
  const char * ret = NULL;
  static /* __thread */ char buf [XSTR_EMAXLEN];

# if defined (__APPLE__) || defined (_AIX) || defined (__MINGW32__) || defined (__MINGW64__)
#  if defined (__MINGW32__) || defined (__MINGW64__)
  if (0 == strerror_s (buf, sizeof (buf), errnum)) /*LINTOK: xstrerror_l*/
    ret = buf;
#  else
  if (0 == strerror_r (errnum, buf, sizeof (buf))) /*LINTOK: xstrerror_l*/
    ret = buf;
#  endif
# else
#  if defined (__NetBSD__)
  locale_t loc = LC_GLOBAL_LOCALE;
#  else
  locale_t loc = uselocale ((locale_t)0);
#  endif
  locale_t copy = loc;
  if (LC_GLOBAL_LOCALE == copy)
    copy = duplocale (copy);

  if ((locale_t)0 != copy)
    {
      ret = strerror_l (errnum, copy); /*LINTOK: xstrerror_l*/
      if (LC_GLOBAL_LOCALE == loc)
        freelocale (copy);
    }
# endif

  if (! ret)
    {
      int n_buf = snprintf (buf, sizeof (buf), "Unknown error %d", errnum);
      if (0 > n_buf || (size_t)n_buf >= sizeof (buf))
        {
          (void)fprintf (stderr, "FATAL: snprintf buffer overflow at %s[%s:%d]\n", __func__, __FILE__, __LINE__);
          exit (EXIT_FAILURE);
        }

      ret = buf;
    }

  errno = saved;
  return ret;
}
#else
# define xstrerror_l strerror
#endif

static ULONG
xstrtoUL (char *nptr, char **endptr, int base)
{
  char *p = nptr;
  ULONG result = 0;
  int any = 0;
  int neg = 0;

  if ((base != 0 && (base < 2 || base > 36)))
    {
      if (endptr)
        *endptr = nptr;

      errno = EINVAL;

      return 0;
    }

  while (*p && isspace((unsigned char)*p))
    p++;

  if (*p == '+' || *p == '-')
    {
      neg = (*p == '-');
      p++;
    }

  if (base == 0)
    {
      if (*p == '0')
        {
          if ((p[1] == 'z' || p[1] == 'Z'))
            {
              int next;

              if (p[2] >= '0' && p[2] <= '9')
                next = p[2] - '0';
              else if (p[2] >= 'a' && p[2] <= 'z')
                next = p[2] - 'a' + 10;
              else if (p[2] >= 'A' && p[2] <= 'Z')
                next = p[2] - 'A' + 10;
              else
                next = -1;

              if (next >= 0 && next < 36) //-V560
                {
                  base = 36;
                  p   += 2;
                }
              else
                base = 0; //-V1048
            }
          else if ((p[1] == 'x' || p[1] == 'X'))
            {
              int next;

              if (p[2] >= '0' && p[2] <= '9')
                next = p[2] - '0';
              else if (p[2] >= 'a' && p[2] <= 'z')
                next = p[2] - 'a' + 10;
              else if (p[2] >= 'A' && p[2] <= 'Z')
                next = p[2] - 'A' + 10;
              else
                next = -1;

              if (next >= 0 && next < 16)
                {
                  base = 16;
                  p   += 2;
                }
              else
                base = 0; //-V1048
            }
          else if ((p[1] == 'b' || p[1] == 'B'))
            {
              int next;

              if (p[2] >= '0' && p[2] <= '1')
                next = p[2] - '0';
              else
                next = -1;

              if (next >= 0 && next < 2) //-V560
                {
                  base = 2;
                  p   += 2;
                }
              else
                base = 0; //-V1048
            }
          else if ((p[1] == 't' || p[1] == 'T'))
            {
              int next;

              if (p[2] >= '0' && p[2] <= '2')
                next = p[2] - '0';
              else
                next = -1;

              if (next >= 0 && next < 3) //-V560
                {
                  base = 3;
                  p   += 2;
                }
              else
                base = 0; //-V1048
            }

          else if ((p[1] == 'o' || p[1] == 'O'))
            {
              int next;

              if (p[2] >= '0' && p[2] <= '7')
                next = p[2] - '0';
              else
                next = -1;

              if (next >= 0 && next < 8) //-V560
                {
                  base = 8;
                  p   += 2;
                }
              else
                base = 0; //-V1048
            }

          if (base == 0)
            base = 8;
        }
      else
        base = 10;
    }
  else if (base == 2)
    {
      if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B'))
        {
          int next;

          if (p[2] >= '0' && p[2] <= '1')
            next = p[2] - '0';
          else
            next = -1;

          if (next >= 0 && next < 2) //-V560
            p += 2;
        }
    }
  else if (base == 3)
    {
      if (p[0] == '0' && (p[1] == 't' || p[1] == 'T'))
        {
          int next;

          if (p[2] >= '0' && p[2] <= '2')
            next = p[2] - '0';
          else
            next = -1;

          if (next >= 0 && next < 3) //-V560
            p += 2;
        }
    }
  else if (base == 16)
    {
      if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        {
          int next;

          if (p[2] >= '0' && p[2] <= '9')
            next = p[2] - '0';
          else if (p[2] >= 'a' && p[2] <= 'z')
            next = p[2] - 'a' + 10;
          else if (p[2] >= 'A' && p[2] <= 'Z')
            next = p[2] - 'A' + 10;
          else
            next = -1;

          if (next >= 0 && next < 16)
            p += 2;
        }
    }

  for (;; p++)
    {
      int d;
      unsigned char c = (unsigned char)*p;

      if (c >= '0' && c <= '9')
        d = c - '0';
      else if (c >= 'a' && c <= 'z')
        d = c - 'a' + 10;
      else if (c >= 'A' && c <= 'Z')
        d = c - 'A' + 10;
      else
        break;

      if (d >= base)
        break;

      any = 1;

#if defined (USE_LONG_LONG)
      if (result  > ULLONG_MAX / (unsigned long long)base ||
         (result == ULLONG_MAX / (unsigned long long)base &&
         (unsigned long long)d > ULLONG_MAX % (unsigned long long)base))
#else
      if (result  > ULONG_MAX / (unsigned long)base ||
         (result == ULONG_MAX / (unsigned long)base &&
         (unsigned long)d > ULONG_MAX % (unsigned long)base))
#endif
        {
          errno = ERANGE;
          p++;

          while (*p)
            {
              c = (unsigned char)*p;

              if (c >= '0' && c <= '9')
                d = c - '0';
              else if (c >= 'a' && c <= 'z')
                d = c - 'a' + 10;
              else if (c >= 'A' && c <= 'Z')
                d = c - 'A' + 10;
              else
                break;

              if (d >= base)
                break;

              p++;
            }

          if (endptr)
            *endptr = p;

#if defined (USE_LONG_LONG)
          return ULLONG_MAX;
#else
          return ULONG_MAX;
#endif
        }

#if defined (USE_LONG_LONG)
      result = result * (unsigned long long)base + (unsigned long long)d;
#else
      result = result * (unsigned long)base + (unsigned long)d;
#endif
    }

  if (!any)
    {
      if (endptr)
        *endptr = nptr;

      return 0;
    }

  if (neg)
#if defined (USE_LONG_LONG)
    result = (unsigned long long)(-(long long)result);
#else
    result = (unsigned long)(-(long)result);
#endif

  if (endptr)
    *endptr = p;

  return result;
}

static ULONG do_assignment_operator(char **str, char *var_name);
static ULONG parse_expression(char *str);  /* Top-level interface to parser */
static ULONG assignment_expr(char **str);  /* Assignments =, +=, *=, etc    */
static ULONG logical_or_expr(char **str);  /* Logical OR '||'               */
static ULONG logical_and_expr(char **str); /* Logical AND '&&'              */
static ULONG or_expr(char **str);          /* OR  '|'                       */
static ULONG xor_expr(char **str);         /* XOR '^'                       */
static ULONG and_expr(char **str);         /* AND '&'                       */
static ULONG equality_expr(char **str);    /* Equality ==, !=               */
static ULONG relational_expr(char **str);  /* Relational <, >, <=, >=       */
static ULONG shift_expr(char **str);       /* Shifts <<, >>                 */
static ULONG add_expression(char **str);   /* Addition/Subtraction +, -     */
static ULONG term(char **str);             /* Multiplication/Division *,%,/ */
static ULONG factor(char **str);           /* Negation, Logical NOT ~, !    */
static ULONG get_value(char **str);

/*
 * Variables are kept in a simple singly-linked list. Not high
 * performance, but it's also an extremely small implementation.
 * New variables get added to the head of the list.  Variables
 * can be unset/removed by assigning no value (e.g. 'var=').
 */

typedef struct variable
{
  char *name;
  ULONG value;
  struct variable *next;
} variable;

static variable dummy = {
  NULL, 0L, NULL
};
static variable *vars = &dummy;

/*
 * This is a hook for external read-only variables. If it is set and we
 * don't find a variable name in our name space, we call it to look for
 * the variable.  If it finds the name, it fills in val and returns 1.
 * If it returns 0, it didn't find the variable.
 */

static int (*external_var_lookup) (const char *name, ULONG *val) = (int (*)(const char *, ULONG *))NULL;

/*
 * This very ugly function declaration is for the function
 * set_var_lookup_hook which accepts one argument, "func", which
 * is a pointer to a function that returns int (and accepts a
 * char * and ULONG *).  set_var_lookup_hook returns a pointer to
 * a function that returns int and accepts char * and ULONG *.
 *
 * It's very ugly looking but fairly basic in what it does.  You
 * pass in a function to set as the variable name lookup up hook
 * and it passes back to you the old function (which you should
 * call if it is non-NULL and your function fails to find the
 * variable name).
 */

static int(
  *set_var_lookup_hook(
    int ( *func ) (
      const char *name,
      ULONG *val))) (
  const char *name,
  ULONG *val)
{
  int (*old_func) (const char *name, ULONG *val) = external_var_lookup;

  external_var_lookup = func;

  return old_func;
}

/*
 * last_result is equal to the result of the last expression and
 * expressions can refer to it as '.' (just like bc).
 */

static ULONG last_result   = 0;
static int suppress_output = 0;
static int unset_silent    = 0;

static char *
convert_base_string(ULONG value, int base, char *buf, int buf_size)
{
  const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char *ptr = &buf[(size_t)buf_size - 1];

  *ptr = '\0';

  if (value == 0)
    {
      *(--ptr) = '0';

      return ptr;
    }

  while (value > 0 && ptr > buf)
    {
      *(--ptr) = digits[(unsigned char)(value % (ULONG)base)];
      value /= (ULONG)base;
    }

  return ptr;
}

static char *
get_binary_string(ULONG value)
{
  static char bin_buf[65];
  char *ptr = bin_buf;

  (void)snprintf(bin_buf, sizeof(bin_buf), PFC_INT64, PBI_64(value));

  while (*ptr == '0' && *(ptr + 1) != '\0')
    ptr++;

  return ptr;
}

static void
print_result(ULONG value)
{
  char dec_str[128];
  char oct_str[30];
  char hex_str[25];
  char ter_str[50];
  char b36_str[20];
  char bin_str[80];
  char extra_info[100] = "";
  char char_repr[sizeof(ULONG) + 1];
  int i;
  int has_signed_info = 0;
  int printable_chars_count = 0;
  const char *ternary_val;
  const char *base36_val;
  size_t line_len = 4;

  /*LINTED: E_CONSTANT_CONDITION*/
  if (sizeof(ULONG) == 8)
    (void)snprintf(dec_str, sizeof(dec_str), "DEC: %llu", value);
  else
    (void)snprintf(dec_str, sizeof(dec_str), "DEC: %lu", (unsigned long)value);

  if ((LONG)value < 0)
    { /*LINTED: E_CONSTANT_CONDITION*/
      if (sizeof(ULONG) == 8)
        (void)snprintf(extra_info, sizeof(extra_info), " (signed: %lld)", (LONG)value);
      else
        (void)snprintf(extra_info, sizeof(extra_info), " (signed: %ld)", (long)value);

      has_signed_info = 1;
    }

  for (i = 0; i < (int)sizeof(ULONG); i++)
    {
      ULONG ch = (value >> (i * CHAR_BIT)) & 0xFF;

      if (isprint((unsigned char)ch))
        {
          char_repr[sizeof(ULONG) - 1 - (size_t)i] = (char)ch;
          printable_chars_count++;
        }
      else
        char_repr[sizeof(ULONG) - 1 - (size_t)i] = '.';
    }

  char_repr[sizeof(ULONG)] = '\0';

  if (printable_chars_count > 0)
    {
      if (has_signed_info)
        (void)snprintf(extra_info + strlen(extra_info), sizeof(extra_info) - strlen(extra_info), " (char: '%s')", char_repr);
      else
        (void)snprintf(extra_info, sizeof(extra_info), " (char: '%s')", char_repr);
    }

  (void)strncat(dec_str, extra_info, sizeof(dec_str) - strlen(dec_str) - 1);

  (void)snprintf(oct_str, sizeof(oct_str), "OCT: 0o%llo", value);

  if (value == 0)
    (void)snprintf(hex_str, sizeof(hex_str), "HEX: 0x0");
  else if (value <= 0xFFFFFFFFUL)
    (void)snprintf(hex_str, sizeof(hex_str), "HEX: 0x%lx", (unsigned long)value);
  else
    (void)snprintf(hex_str, sizeof(hex_str), "HEX: 0x%llx", value);

  char ternary_str_buf[45];
  ternary_val = convert_base_string(value, 3, ternary_str_buf, sizeof(ternary_str_buf));
  (void)snprintf(ter_str, sizeof(ter_str), "TER: 0t%s", ternary_val);

  char base36_str_buf[16];
  base36_val = convert_base_string(value, 36, base36_str_buf, sizeof(base36_str_buf));
  (void)snprintf(b36_str, sizeof(b36_str), "B36: 0z%s", base36_val);

  (void)snprintf(bin_str, sizeof(bin_str), "BIN: 0b%s", get_binary_string(value));

  const char *fields[] = {
    dec_str, oct_str,
    hex_str, ter_str,
    b36_str, bin_str,
    NULL
  };

  (void)fprintf(stdout, "    ");

  for (i = 0; fields[i] != NULL; i++)
    {
      size_t field_len = strlen(fields[i]);

      if (line_len > 4 && line_len + field_len > 80)
        {
          (void)fprintf(stdout, "\n    ");
          line_len = 4;
        }

      (void)fprintf(stdout, "%s", fields[i]);

      line_len += field_len;

      if (fields[i+1] != NULL)
        {
          (void)fprintf(stdout, " ");
          line_len++;
        }
    }

  (void)fprintf(stdout, "\n");
}

static int
builtin_vars(const char *name, ULONG *val)
{
  static int isbe;
  static int endianed = 0;

  if (endianed == 0)
    {
      int tmp = 1;
      isbe = (*((char *)&tmp) ? 0 : 1);
      endianed = 1;
    }

  /* NB: Keep in sync with builtin_var_names */
  if (strcmp(name, "ENDIAN_BIG") == 0)
    *val = (ULONG)isbe;
  else if (strcmp(name, "ENDIAN_LITTLE") == 0)
    *val = (ULONG)!isbe;
  else if (strcmp(name, "time") == 0)
    *val = (ULONG)time(NULL);
  else if (strcmp(name, "rand") == 0)
#if defined (__OpenBSD__) && defined (OpenBSD) && (OpenBSD >= 200811)
    *val = (ULONG)arc4random_uniform((uint32_t)RAND_MAX + 1);
#else
    *val = (ULONG)rand();
#endif
  else if (strcmp(name, "dbg") == 0)
    *val = 0x82969;
  else if (strcmp(name, "pid") == 0)
    *val = (ULONG)getpid();
  else if (strcmp(name, "uid") == 0)
    *val = (ULONG)getuid();
  else if (strcmp(name, "gid") == 0)
    *val = (ULONG)getgid();
  else if (strcmp(name, "errno") == 0)
    *val = (ULONG)errno;
  else if (strcmp(name, "ULLONG_MAX") == 0)
    *val = (ULONG)ULLONG_MAX;
  else if (strcmp(name, "LLONG_MAX") == 0)
    *val = (ULONG)LLONG_MAX;
  else if (strcmp(name, "LLONG_MIN") == 0)
    *val = (ULONG)LLONG_MIN;
  else if (strcmp(name, "LONG_MAX") == 0)
    *val = (ULONG)LONG_MAX;
  else if (strcmp(name, "LONG_MIN") == 0)
    *val = (ULONG)LONG_MIN;
  else if (strcmp(name, "INT_MAX") == 0)
    *val = (ULONG)INT_MAX;
  else if (strcmp(name, "INT_MIN") == 0)
    *val = (ULONG)INT_MIN;
  else if (strcmp(name, "INPUT_BUFF") == 0)
    *val = (ULONG)INPUT_BUFF;
  else if (strcmp(name, "ARG_MAX") == 0)
    *val = (ULONG)sysconf(_SC_ARG_MAX);
  else if (strcmp(name, "CHILD_MAX") == 0)
    *val = (ULONG)sysconf(_SC_CHILD_MAX);
  else if (strcmp(name, "OPEN_MAX") == 0)
    *val = (ULONG)sysconf(_SC_OPEN_MAX);
  else if (strcmp(name, "PATH_MAX") == 0)
    *val = (ULONG)pathconf("/", _PC_PATH_MAX);
  else if (strcmp(name, "NAME_MAX") == 0)
    *val = (ULONG)pathconf(".", _PC_NAME_MAX);
  else if (strcmp(name, "CHAR_BIT") == 0)
    *val = (ULONG)CHAR_BIT;
  else if (strcmp(name, "CHAR_MAX") == 0)
    *val = (ULONG)CHAR_MAX;
  else if (strcmp(name, "CHAR_MIN") == 0)
    *val = (ULONG)CHAR_MIN;
  else if (strcmp(name, "SCHAR_MAX") == 0)
    *val = (ULONG)SCHAR_MAX;
  else if (strcmp(name, "SCHAR_MIN") == 0)
    *val = (ULONG)SCHAR_MIN;
  else if (strcmp(name, "UCHAR_MAX") == 0)
    *val = (ULONG)UCHAR_MAX;
#if defined (PIPE_BUF)
  else if (strcmp(name, "PIPE_BUF") == 0)
    *val = (ULONG)PIPE_BUF;
#endif
#if defined (_PC_FILESIZEBITS)
  else if (strcmp(name, "FILESIZEBITS") == 0)
    *val = (ULONG)pathconf(".", _PC_FILESIZEBITS);
#endif
  else if (strcmp(name, "NULL") == 0)
    *val = 0;
  else if (strcmp(name, "nil") == 0)
    *val = 0;
#if defined (WORD_BIT)
  else if (strcmp(name, "WORD_BIT") == 0)
    *val = (ULONG)WORD_BIT;
#endif
#if defined (LONG_BIT)
  else if (strcmp(name, "LONG_BIT") == 0)
    *val = (ULONG)LONG_BIT;
#endif
#if defined (PAGESIZE)
  else if (strcmp(name, "PAGESIZE") == 0)
    *val = (ULONG)PAGESIZE;
#endif
#if defined (PAGE_SIZE)
  else if (strcmp(name, "PAGE_SIZE") == 0)
    *val = (ULONG)PAGE_SIZE;
#endif
  else if (strcmp(name, "SHRT_MAX") == 0)
    *val = (ULONG)SHRT_MAX;
  else if (strcmp(name, "SHRT_MIN") == 0)
    *val = (ULONG)SHRT_MIN;
  else if (strcmp(name, "USHRT_MAX") == 0)
    *val = (ULONG)USHRT_MAX;
  else if (strcmp(name, "UINT_MAX") == 0)
    *val = (ULONG)UINT_MAX;
  else if (strcmp(name, "ULONG_MAX") == 0)
    *val = (ULONG)ULONG_MAX;
  else if (strcmp(name, "RAND_MAX") == 0)
    *val = (ULONG)RAND_MAX;
  else if (strcmp(name, "EOF") == 0)
    *val = (ULONG)EOF;
  else if (strcmp(name, "STDIN_FILENO") == 0)
    *val = (ULONG)STDIN_FILENO;
  else if (strcmp(name, "STDOUT_FILENO") == 0)
    *val = (ULONG)STDOUT_FILENO;
  else if (strcmp(name, "STDERR_FILENO") == 0)
    *val = (ULONG)STDERR_FILENO;
  else if (strcmp(name, "sizeof_char") == 0)
    *val = (ULONG)sizeof(char);
  else if (strcmp(name, "sizeof_short") == 0)
    *val = (ULONG)sizeof(short);
  else if (strcmp(name, "sizeof_int") == 0)
    *val = (ULONG)sizeof(int);
  else if (strcmp(name, "sizeof_long") == 0)
    *val = (ULONG)sizeof(long);
  else if (strcmp(name, "sizeof_ll") == 0)
    *val = (ULONG)sizeof(long long);
  else if (strcmp(name, "sizeof_void") == 0)
    *val = (ULONG)sizeof(void *);
  else
    return 0;

  return 1;
}

static const char *
builtin_var_names [] =
{
  "ARG_MAX",
  "CHAR_BIT",
  "CHAR_MAX",
  "CHAR_MIN",
  "CHILD_MAX",
  "dbg",
  "ENDIAN_BIG",
  "ENDIAN_LITTLE",
  "EOF",
  "errno",
#if defined (_PC_FILESIZEBITS)
  "FILESIZEBITS",
#endif
  "gid",
  "INT_MAX",
  "INT_MIN",
  "INPUT_BUFF",
  "LLONG_MAX",
  "LLONG_MIN",
#if defined (LONG_BIT)
  "LONG_BIT",
#endif
  "LONG_MAX",
  "LONG_MIN",
  "NAME_MAX",
  "nil",
  "NULL",
  "OPEN_MAX",
#if defined (PAGESIZE)
  "PAGESIZE",
#endif
#if defined (PAGE_SIZE)
  "PAGE_SIZE",
#endif
  "PATH_MAX",
  "pid",
#if defined (PIPE_BUF)
  "PIPE_BUF",
#endif
  "rand",
  "RAND_MAX",
  "SCHAR_MAX",
  "SCHAR_MIN",
  "SHRT_MAX",
  "SHRT_MIN",
  "sizeof_char",
  "sizeof_int",
  "sizeof_ll",
  "sizeof_long",
  "sizeof_short",
  "sizeof_void",
  "STDERR_FILENO",
  "STDIN_FILENO",
  "STDOUT_FILENO",
  "time",
  "UCHAR_MAX",
  "uid",
  "UINT_MAX",
  "ULLONG_MAX",
  "ULONG_MAX",
  "USHRT_MAX",
#if defined (WORD_BIT)
  "WORD_BIT",
#endif
  NULL
};

typedef struct
{
  const char *name;
  ULONG value;
} var_entry;

typedef enum
{
  USER_VARS,
  BUILTIN_VARS
} varquery_type;

static int
compare_var_entries(const void *a, const void *b)
{
  const var_entry *var_a = (const var_entry *)a;
  const var_entry *var_b = (const var_entry *)b;

  return strcmp(var_a->name, var_b->name);
}

static var_entry *
resize_var_entries(var_entry *entries, int count, int *capacity)
{
  if (count >= *capacity)
    {
      var_entry *new_entries;
      *capacity *= 2;
      new_entries = realloc(entries, (size_t)*capacity * sizeof(var_entry));

      if (new_entries == NULL)
        {
          (void)fprintf(stderr, "ERROR: memory reallocation failed\n");
          FREE(entries);

          return NULL;
        }

      return new_entries;
    }

  return entries;
}

static inline variable *
lookup_var(const char *name)
{
  variable *v;

  for (v = vars; v; v = v->next)
    if (v->name && strcmp(v->name, name) == 0)
      return v;

  return NULL;
}

static int
is_reserved_name(const char *name)
{
  if (name == NULL)
    return 0;

  if (strcmp(name, "vars") == 0
   || strcmp(name, "regs") == 0
   || strcmp(name, "help") == 0)
    return 1;

  return 0;
}

static variable *
add_var(char *name, ULONG value)
{
  variable *v;
  ULONG tmp;

  /* First make sure this isn't a reserved name or keyword */

  if (is_reserved_name(name))
    {
      (void)fprintf(stderr, "ERROR: can't assign/create '%s', is a reserved name.\n", name);

      return NULL;
    }

  /* Next make sure this isn't an external read-only variable */

  if (external_var_lookup)
    if (external_var_lookup(name, &tmp) != 0)
      {
        (void)fprintf(stderr, "Can't assign/create '%s', it is a read-only variable\n", name);

        return NULL;
      }

  v = malloc(sizeof ( variable ));

  if (v == NULL)
    {
      (void)fprintf(stderr, "No memory to add variable '%s'\n", name);

      return NULL;
    }

  v->name  = strdup(name);
  v->value = value;
  v->next  = vars;

  vars = v; /* Set head of list to the new guy */

  return v;
}

/*
 * This routine and the companion get_var() are external
 * interfaces to the variable manipulation routines.
 */

#if defined (EXTERNAL)
static void
set_var(char *name, ULONG val)
{
  variable *v;

  v = lookup_var(name);

  if (v != NULL)
    v->value = val;
  else
    (void)add_var(name, val);
}
#endif

/*
 * This function returns 1 on success of finding
 * a variable and 0 on failure to find a variable.
 * If a variable is found, val is filled with its
 * value.
 */

static int
get_var(const char *name, ULONG *val)
{
  variable *v;

  v = lookup_var(name);
  if (v != NULL)
    {
      *val = v->value;

      return 1;
    }
  else if (external_var_lookup != NULL)
    return external_var_lookup(name, val);

  return 0;
}

static int
is_register(const char *name)
{
  if (name == NULL)
    return 0;

  if (strcmp(name, "GC")  == 0
   || strcmp(name, "GS")  == 0
   || strcmp(name, "GI")  == 0
   || strcmp(name, "GL")  == 0
   || strcmp(name, "GLL") == 0
   || strcmp(name, "GT")  == 0)
    return 1;

  return 0;
}

static ULONG
truncate_register(const char *name, ULONG value)
{
  if (strcmp(name, "GC") == 0)
    return (unsigned char)value;

  if (strcmp(name, "GS") == 0)
    return (unsigned short)value;

  if (strcmp(name, "GI") == 0) /*LINTED: E_CAST_INT_TO_SMALL_INT*/
    return (unsigned int)value;

  if (strcmp(name, "GL") == 0)
    return (unsigned long)value;

  return value;
}

static void
list_vars(varquery_type type)
{
  variable *v;
  ULONG val;
  int i;
  int count = 0;
  int capacity = 32;

  var_entry *entries = malloc((size_t)capacity * sizeof(var_entry));

  if (entries == NULL)
    {
      (void)fprintf(stderr, "ERROR: memory allocation failed\n");

      return;
    }

  if (type == USER_VARS)
    {
      for (v = vars; v; v = v->next)
        if (v->name && !is_register(v->name))
          {
            entries = resize_var_entries(entries, count, &capacity);

            if (entries == NULL)
              return;

            entries[count].name = v->name;
            entries[count].value = v->value;

            count++;
          }
    }
  else if (type == BUILTIN_VARS)
    {
      for (i = 0; builtin_var_names[i] != NULL; i++)
        if (get_var(builtin_var_names[i], &val))
          {
            entries = resize_var_entries(entries, count, &capacity);

            if (entries == NULL)
              return;

            entries[count].name = builtin_var_names[i];
            entries[count].value = val;

            count++;
          }
    }
  else
    {
      (void)fprintf(stderr, "FATAL: Bugcheck: unknown varquery_type at %s[%s:%d]\n",
                    __FILE__, __func__, __LINE__);
      abort();
    }

  qsort(entries, (size_t)count, sizeof(var_entry), compare_var_entries);

  if (type == USER_VARS)
    {
      if (count == 0)
        {
          (void)fprintf(stdout, "No user variables defined.\n");
          FREE(entries);

          return;
        }

      (void)fprintf(stdout, "User variables:\n");
    }
  else if (type == BUILTIN_VARS) //-V547
    (void)fprintf(stdout, "The following read-only builtin variables are defined:\n");
  else
    {
      (void)fprintf(stderr, "FATAL: Bugcheck: unknown varquery_type at %s[%s:%d]\n",
                    __FILE__, __func__, __LINE__);
      abort();
    }

  for (i = 0; i < count; i++)
    {
      (void)fprintf(stdout, "  %s:\n", entries[i].name);
      print_result(entries[i].value);
    }

  FREE(entries);
}

/* Special formatting of GT time register */

static void
print_time_reg(const char *name, ULONG value)
{
  time_t time_val = (time_t)value;
  struct tm *tm_info = localtime(&time_val);
  size_t size = 64;
  char *buf = NULL;
  char *new_buf = NULL;
  size_t len;

  do {
    new_buf = realloc(buf, size);

    if (new_buf == NULL) {
      FREE(buf);
      print_result(value);

      return;
    }

    buf = new_buf;
    len = strftime(buf, size, "%c", tm_info);
    size *= 2;
  } while (len == 0);

  (void)fprintf(stdout, "  %s: %s\n", name, buf);
  FREE(buf);
}

/* Custom sort order for registers */

static int
get_reg_order(const char *name)
{
  if (strcmp(name, "GT") == 0)
    return 0;

  if (strcmp(name, "GC") == 0)
    return 1;

  if (strcmp(name, "GS") == 0)
    return 2;

  if (strcmp(name, "GI") == 0)
    return 3;

  if (strcmp(name, "GL") == 0)
    return 4;

  if (strcmp(name, "GLL") == 0)
    return 5;

  return 6;
}

static int
compare_reg_entries(const void *a, const void *b)
{
  const var_entry *var_a = (const var_entry *)a;
  const var_entry *var_b = (const var_entry *)b;

  return get_reg_order(var_a->name) - get_reg_order(var_b->name);
}

static void
list_regs(void)
{
  variable *v;
  int i;
  int count = 0;
  var_entry entries[6] = {0};

  for (v = vars; v; v = v->next)
    if (v->name && is_register(v->name))
      if (count < 6)
        {
          entries[count].name = v->name;
          entries[count].value = v->value;
          count++;
        }

  qsort(entries, (size_t)count, sizeof(var_entry), compare_reg_entries);

  (void)fprintf(stdout, "Registers:\n");

  for (i = 0; i < count; i++)
    {
      if (strcmp(entries[i].name, "GT") == 0)
        {
          print_time_reg(entries[i].name, entries[i].value);
        }
      else
        {
          (void)fprintf(stdout, "  %s:\n", entries[i].name);
          print_result(entries[i].value);
        }
    }
}

static void
list_user_vars(void)
{
  list_vars(USER_VARS);
}

static void
list_builtin_vars(void)
{
  list_vars(BUILTIN_VARS);
}

static const char *
squash(const char *s)
{
  static char buf[INPUT_BUFF];
  char *d = buf;
  int in_space = 0;

 if (s == NULL)
   {
     buf[0] = '\0';

     return buf;
   }

  while (*s && d < (buf + (sizeof(buf) - 1)))
    {
      if (*s == ' ' || *s == '\t')
        {
          if (!in_space)
            {
              *d++ = ' ';
              in_space = 1;
            }
        } /*LINTED: E_NOP_IF_STMT*/
      else if (*s == '"' || *s == '\'');
      else
        {
          *d++ = *s;
          in_space = 0;
        }

      s++;
    }

  *d = '\0';

  return buf;
}

static void
print_herald(void)
{
  char oshitbuf[6];

#if defined (__clang_version__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunreachable-code"
#endif

  /*LINTED: E_CONSTANT_CONDITION*/
  if (PC_VERSION_OSHIT > 0) /*NOTREACHED*/ /* unreachable */
    (void)snprintf(oshitbuf, sizeof(oshitbuf), "-%d", PC_VERSION_OSHIT);
  else /*NOTREACHED*/ /* unreachable */
    oshitbuf[0] = '\0';

  (void)fprintf(stdout, "%s v%d.%d.%d%s%s%s%s ready.\n",
                PC_SOFTWARE_NAME, PC_VERSION_MAJOR, PC_VERSION_MINOR, PC_VERSION_PATCH,
                oshitbuf, (void *)PC_SOFTWARE_DATE ? " (" : "",
                (void *)PC_SOFTWARE_DATE ? squash(PC_SOFTWARE_DATE) : "",
                (void *)PC_SOFTWARE_DATE ? ")" : "");


#if defined (__clang_version__)
# pragma clang diagnostic pop
#endif
}

static void
do_input(void)
{
  ULONG value;
#if defined(WITH_READLINE) || defined(WITH_LIBEDIT)
  char *line;

  while ((line = readline("")) != NULL)
    {
      char *ptr = line;
#elif defined(WITH_LINENOISE)
  char *line;

  while ((line = linenoise("")) != NULL)
    {
      char *ptr = line;
#else
    char buff[INPUT_BUFF];

    while (fgets(buff, INPUT_BUFF, stdin) != NULL)
      {
        char *ptr = buff;

        if (strlen(ptr) > 0 && ptr[strlen(ptr) - 1] == '\n')
          ptr[strlen(ptr) - 1] = '\0';
#endif

        while (*ptr && isspace((unsigned char)*ptr))
          ptr++;

        if (*ptr == '\0')
          {
#if defined(WITH_READLINE) || defined(WITH_LIBEDIT)
            FREE(line);
#elif defined(WITH_LINENOISE)
            linenoiseFree(line);
#endif
            continue;
          }

        char *end = ptr + strlen(ptr) - 1;

        while (end > ptr && isspace((unsigned char)*end))
          *end-- = '\0';

#if defined(WITH_READLINE) || defined(WITH_LIBEDIT)
        add_history(line);
#elif defined(WITH_LINENOISE)
        linenoiseHistoryAdd(line);
#endif

        if (strcmp(ptr, "vars") == 0)
          list_user_vars();
        else if (strcmp(ptr, "regs") == 0)
          list_regs();
        else if (strcmp(ptr, "help") == 0)
          {
            list_builtin_vars();
            list_regs();
            list_user_vars();
          }
        else
          {
            value = parse_expression(ptr);

            if (!suppress_output)
              print_result(value);
          }

#if defined(WITH_READLINE) || defined(WITH_LIBEDIT)
        FREE(line);
#elif defined(WITH_LINENOISE)
        linenoiseFree(line);
#endif
    }
}

static void
parse_args(int argc, char *argv[])
{
  size_t i, len;
  char *buff, *ptr, *end;
  ULONG value;

  for (i = 1, len = 0; i < (size_t)argc; i++)
    len += strlen(argv[i]) + 1;

  len++;

  buff = malloc((ULONG)len * sizeof ( char ));

  if (buff == NULL)
    return;

  buff[0] = '\0';

  for (i = 1; i < (size_t)argc; i++)
    {
      (void)strncat(buff, argv[i], len - strlen(buff) - 1);
      (void)strncat(buff, " ", len - strlen(buff) - 1);
    }

  ptr = buff;

  while (*ptr && isspace((unsigned char)*ptr))
    ptr++;

  end = ptr + strlen(ptr) - 1;

  while (end > ptr && isspace((unsigned char)*end))
    *end-- = '\0';

  if (strcmp(ptr, "vars") == 0)
    {
      list_user_vars();
      FREE(buff);

      return;
    }

  if (strcmp(ptr, "regs") == 0)
    {
      list_regs();
      FREE(buff);

      return;
    }

  if (strcmp(ptr, "help") == 0)
    {
      list_builtin_vars();
      FREE(buff);

      return;
    }

  value = parse_expression(buff);

  if (!suppress_output)
    print_result(value);

  FREE(buff);
}

int
main(int argc, char *argv[])
{
#if !defined(NO_LOCALE)
  (void)setlocale (LC_ALL, "");
#endif

  (void)set_var_lookup_hook(builtin_vars);

  (void)add_var("GC" , 0);
  (void)add_var("GS",  0);
  (void)add_var("GI",  0);
  (void)add_var("GL",  0);
  (void)add_var("GLL", 0);
  (void)add_var("GT",  0);

  if (argc > 1)
    parse_args(argc, argv);
  else
    {
      if (isatty(STDIN_FILENO))
        print_herald();

      do_input();
    }

  return EXIT_SUCCESS;
}

static char *
skipwhite(char *str)
{
  if (str == NULL)
    return NULL;

  while (*str && ( *str == ' ' || *str == '\t' || *str == '\n' || *str == '\f' ))
    str++;

  return str;
}

static ULONG
parse_expression(char *str)
{
  ULONG val;
  char *ptr = str;

  ptr = skipwhite(ptr);

  if (ptr == NULL || *ptr == '\0')
    return last_result;

  if (strcmp(ptr, "GT") == 0)
    if (get_var("GT", &val))
      {
        print_time_reg("GT", val);
        suppress_output = 1;
        return val;
      }

  val = assignment_expr(&ptr);
  ptr = skipwhite(ptr);

  while (ptr != NULL && (*ptr == SEMI_COLON))
    {
      ptr++;

      if (*ptr == '\0') /* Reached the end of the string, stop parsing */
        {
          suppress_output = 1;
          break;
        }

      val = assignment_expr(&ptr);
    }

  last_result = val;

  return val;
}

static int
remove_var(char *name)
{
  variable *v, *prev = NULL;

  if (name == NULL)
    return 0;

  for (v = vars; v; prev = v, v = v->next)
    if (v->name && strcmp(v->name, name) == 0)
      {
        if (prev)
          prev->next = v->next;
        else
          vars = v->next;

        FREE(v->name);
        FREE(v);

        return 1;
      }

  return 0;
}

#define DEFAULT_LEN 32

static char *
get_var_name(char **str)
{
  int i, len = DEFAULT_LEN;
  char *buff, *tmpbuff;

  if (isalpha((unsigned char)**str) == 0 && **str != '_')
    return NULL;

  buff = malloc((ULONG)len * sizeof ( char ));

  if (buff == NULL)
    return NULL;

  /*
   * First get the variable name
   */

  i = 0;

  while (**str && ( isalnum((unsigned char)**str) || **str == '_' ))
    {
      if (i >= len - 1)
        {
          len     *= 2;
          tmpbuff  = realloc(buff, (ULONG)len);

          if (tmpbuff == NULL)
            {
              FREE(buff);

              return NULL;
            }

          buff = tmpbuff;
        }

      buff[i++] = **str;
      *str      = *str + 1;
    }

  buff[i] = '\0'; /* NULL terminate */

  while (isalnum((unsigned char)**str) || **str == '_') /* Skip over any remaining junk */
    *str = *str + 1;

  return buff;
}

static ULONG
assignment_expr(char **str)
{
  ULONG val;
  char *orig_str;
  char *var_name;
  variable *v;

  *str     = skipwhite(*str);
  orig_str = *str;
  var_name = get_var_name(str);

  if (var_name == NULL)
    {
      *str = orig_str;

      return logical_or_expr(str);
    }

  *str = skipwhite(*str);

  if (*str != NULL && (**str == EQUAL && *( *str + 1 ) != EQUAL))
    {
      char *peek;

      *str = skipwhite(*str + 1); /* Skip the equal sign */
      peek = skipwhite(*str);

      if (peek != NULL && (*peek == '\0' || *peek == SEMI_COLON))
        {
          if (is_register(var_name))
            {
              (void)fprintf(stderr, "ERROR: Cannot unset register '%s'.\n", var_name);
              val  = 0;
              *str = peek;
              suppress_output = 1;
            }
          else
            {
              int existed;

              unset_silent = (*peek == SEMI_COLON);
              existed = remove_var(var_name);

              if (existed && !unset_silent)
                (void)fprintf(stdout, "Variable '%s' unset.\n", var_name);
              else if (!existed && !unset_silent)
                (void)fprintf(stderr, "Warning: No such variable '%s'.\n", var_name);

              val  = 0;
              *str = peek;
              suppress_output = 1;
            }
        }
      else
        {
          val  = assignment_expr(str); /* Go recursive! */

          if (suppress_output) /* RHS was an unset chain */
            {
              int existed = remove_var(var_name);

              if (existed && !unset_silent)
                (void)fprintf(stdout, "Variable '%s' unset.\n", var_name);
            }
          else /* RHS was a normal expression */
            {
              suppress_output = 0; //-V1048

              if ((v = lookup_var(var_name)) == NULL)
                (void)add_var(var_name, val);
              else
                {
                  if (is_register(var_name))
                    v->value = truncate_register(var_name, val);
                  else
                    v->value = val;
                }
            }
        }
    }
  else if (*str != NULL && ((( **str == PLUS || **str == MINUS
        || **str == OR || **str == TIMES || **str == DIVISION
        || **str == MODULO || **str == AND
        || **str == XOR ) && *( *str + 1 ) == EQUAL )
        || strncmp(*str, "<<=", 3) == 0 || strncmp(*str, ">>=", 3) == 0))
    val = do_assignment_operator(str, var_name);
  else
    {
      *str = orig_str;
      val  = logical_or_expr(str); /* No equal sign, get var value */
      *str = skipwhite(*str);

      if (*str == NULL)
        {
          (void)fprintf(stderr, "FATAL: Bugcheck: str == NULL %s[%s:%d]\n",
                        __FILE__, __func__, __LINE__);
          abort();
        }

      if (**str == EQUAL)
        (void)fprintf(stderr, "Left hand side of expression is not assignable.\n");
    }

  if (var_name) //-V547
    FREE(var_name);

  return val;
}

static ULONG
do_assignment_operator(char **str, char *var_name)
{
  ULONG val;
  variable *v;
  char operator;

  operator = **str;

  if (operator == SHIFT_L || operator == SHIFT_R)
    *str = skipwhite(*str + 3);
  else
    *str = skipwhite(*str + 2); /* Skip the assignment operator */

  val = assignment_expr(str); /* Go recursive! */
  v = lookup_var(var_name);
  suppress_output = 0;

  if (v == NULL)
    {
      v = add_var(var_name, 0);

      if (v == NULL)
        return 0;
    }

  if (operator == PLUS)
    {
      if (v->value > (ULONG)-1 - val)
        errno = ERANGE;

      v->value += val;
    }
  else if (operator == MINUS)
    {
      if ((LONG)val > 0 && (LONG)v->value < LLONG_MIN + (LONG)val)
        errno = ERANGE;
      else if ((LONG)val < 0 && (LONG)v->value > LLONG_MAX + (LONG)val)
        errno = ERANGE;

      v->value -= val;
    }
  else if (operator == AND)
    v->value &= val;
  else if (operator == XOR)
    v->value ^= val;
  else if (operator == OR)
    v->value |= val;
  else if (operator == SHIFT_L)
    {
      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", xstrerror_l(errno));
        }

      v->value <<= val;
    }
  else if (operator == SHIFT_R)
    {
      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", xstrerror_l(errno));
        }

      v->value >>= val;
    }
  else if (operator == TIMES)
    {
      if (val != 0 && v->value > (ULONG)-1 / val)
        errno = ERANGE;

      v->value *= val;
    }
  else if (operator == DIVISION)
    {
      if (val == 0) /* Check, but still get the result! */
        {
          errno = EDOM;
          (void)fprintf(stderr, "Warning: %s (Division by zero)\n", xstrerror_l(errno));
          v->value = 0;
        }
      else
        v->value /= val;
    }
  else if (operator == MODULO)
    {
      if (val == 0) /* Check, but still get the result! */
        {
          errno = EDOM;
          (void)fprintf(stderr, "Warning: %s (Modulo by zero)\n", xstrerror_l(errno));
          v->value = 0;
        }
      else
        v->value %= val;
    }
  else
    {
      (void)fprintf(stderr, "Unknown operator: %c\n", operator);
      v->value = 0;
    }

  if (is_register(var_name))
    v->value = truncate_register(var_name, v->value);

  return v->value;
}

static ULONG
logical_or_expr(char **str)
{
  ULONG val, sum = 0;

  suppress_output = 0;

  *str = skipwhite(*str);
  sum  = logical_and_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == OR && *( *str + 1 ) == OR))
    {
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = logical_and_expr(str);
      sum  = ( val || sum );
    }

  return sum;
}

static ULONG
logical_and_expr(char **str)
{
  ULONG val, sum = 0;

  *str = skipwhite(*str);
  sum  = or_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == AND && *( *str + 1 ) == AND))
    {
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = or_expr(str);
      sum  = ( val && sum );
    }

  return sum;
}

static ULONG
or_expr(char **str)
{
  ULONG val, sum = 0;

  *str = skipwhite(*str);
  sum  = xor_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == OR && *( *str + 1 ) != OR))
    {
      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = xor_expr(str);
      sum |= val;
    }

  return sum;
}

static ULONG
xor_expr(char **str)
{
  ULONG val, sum = 0;

  *str = skipwhite(*str);
  sum  = and_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == XOR))
    {
      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = and_expr(str);
      sum ^= val;
    }

  return sum;
}

static ULONG
and_expr(char **str)
{
  ULONG val, sum = 0;

  *str = skipwhite(*str);
  sum  = equality_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == AND && *( *str + 1 ) != AND))
    {
      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = equality_expr(str);
      sum &= val;
    }

  return sum;
}

static ULONG
equality_expr(char **str)
{
  ULONG val, sum = 0;
  char op;

  *str = skipwhite(*str);
  sum  = relational_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (
         ( **str == EQUAL && *( *str + 1 ) == EQUAL ) ||
         ( **str == BANG  && *( *str + 1 ) == EQUAL )))
    {
      op   = **str;
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = relational_expr(str);

      if (op == EQUAL)
        sum = ( sum == val );
      else if (op == BANG) //-V547
        sum = ( sum != val );
    }

  return sum;
}

static ULONG
relational_expr(char **str)
{
  ULONG val, sum = 0;
  char op, equal_to = 0;

  *str = skipwhite(*str);
  sum  = shift_expr(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == LESS_THAN || **str == GREATER_THAN))
    {
      equal_to = 0;
      op = **str;

      if (*( *str + 1 ) == EQUAL)
        {
          equal_to = 1;
          *str = *str + 1; /* Skip initial operator */
        }

      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = shift_expr(str);

      /*
       * Notice that we do the relational expressions as signed
       * comparisons.  This is because of expressions like
       * '0 > -1' which would not return the expected value if
       * we did the comparison as unsigned.  This may not always
       * be the desired behavior, but aside from adding casting to
       * expressions, there isn't much of a way around it.
       */

      if (op == LESS_THAN && equal_to == 0)
        sum = ((LONG)sum < (LONG)val );
      else if (op == LESS_THAN && equal_to == 1)
        sum = ((LONG)sum <= (LONG)val );
      else if (op == GREATER_THAN && equal_to == 0)
        sum = ((LONG)sum > (LONG)val );
      else if (op == GREATER_THAN && equal_to == 1)
        sum = ((LONG)sum >= (LONG)val );
    }

  return sum;
}

static ULONG
shift_expr(char **str)
{
  ULONG val, sum = 0;
  char op;

  *str = skipwhite(*str);
  sum  = add_expression(str);
  *str = skipwhite(*str);

  while (*str != NULL && ((strncmp(*str, "<<", 2) == 0) || (strncmp(*str, ">>", 2) == 0)))
    {
      op   = **str;
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = add_expression(str);

      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", xstrerror_l(errno));
        }

      if (op == SHIFT_L)
        sum <<= val;
      else if (op == SHIFT_R)
        sum >>= val;
    }

  return sum;
}

static ULONG
add_expression(char **str)
{
  ULONG val, sum = 0;
  char op;

  *str = skipwhite(*str);
  sum  = term(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == PLUS || **str == MINUS))
    {
      op   = **str;
      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = term(str);

      if (op == PLUS)
        {
          if (sum > (ULONG)-1 - val)
            errno = ERANGE;

          sum += val;
        }
      else if (op == MINUS) //-V547
        {
          if ((LONG)val > 0 && (LONG)sum < LLONG_MIN + (LONG)val)
            errno = ERANGE;

          sum -= val;
        }
    }

  return sum;
}

static ULONG
term(char **str)
{
  ULONG val, sum = 0;
  char op;

  sum  = factor(str);
  *str = skipwhite(*str);

  while (*str != NULL && (**str == TIMES || **str == DIVISION || **str == MODULO))
    {
      op   = **str;
      *str = skipwhite(*str + 1);
      val  = factor(str);

      if (op == TIMES)
        {
          if (val != 0 && sum > (ULONG)-1 / val)
            errno = ERANGE;

          sum *= val;
        }
      else if (op == DIVISION)
        {
          if (val == 0)
            {
              errno = EDOM;
              (void)fprintf(stderr, "Warning: %s (Division by zero)\n", xstrerror_l(errno));
              sum = 0;
            }
          else
            sum /= val;
        }
      else if (op == MODULO) //-V547
        {
          if (val == 0)
            {
              errno = EDOM;
              (void)fprintf(stderr, "Warning: %s (Modulo by zero)\n", xstrerror_l(errno));
              sum = 0;
            }
          else
            sum %= val;
        }
    }

  /*
   * We're at the bottom of the parse.  At this point we either have
   * an operator or we're through with this string.  Otherwise it's
   * an error and we print a message.
   */

  if (*str != NULL && (**str != TIMES && **str != DIVISION
      && **str != MODULO && **str != PLUS //-V560
      && **str != MINUS && **str != OR && **str != AND && **str != XOR
      && **str != BANG && **str != NEGATIVE && **str != TWIDDLE //-V560
      && **str != RPAREN && **str != LESS_THAN && **str != GREATER_THAN
      && **str != SEMI_COLON && strncmp(*str, "<<", 2) != 0
      && strncmp(*str, ">>", 2) && **str != EQUAL && **str != '\0')) //-V526
    {
      (void)fprintf(stderr, "Parsing stopped: unknown operator '%s'\n", *str);

      return sum;
    }

  return sum;
}

static ULONG
factor(char **str)
{
  ULONG val = 0;
  char op = NOTHING, have_special = 0;
  char *var_name, *var_name_ptr;
  variable *v;

  if (**str == NEGATIVE || **str == PLUS || **str == TWIDDLE || **str == BANG)
    {
      op = **str; /* Must be a unary op */

      if (( op == NEGATIVE && *( *str + 1 ) == NEGATIVE ) || /* Look for -- */
          ( op == PLUS     && *( *str + 1 ) == PLUS ))       /* Look for ++ */
        {
          *str = *str + 1;
          have_special = 1;
        }

      *str = skipwhite(*str + 1);
      var_name_ptr = *str; /* Save where the varname should be */
    }

  val  = get_value(str);
  *str = skipwhite(*str);

  /*
   * Now is the time to actually do the unary operation if one was present.
   */

  if (have_special) /* We've got a ++ or -- */
    {
      var_name = get_var_name(&var_name_ptr);

      if (var_name == NULL)
        {
          (void)fprintf(stderr, "Can only use ++/-- on variables.\n");

          return val;
        }

      if ((v = lookup_var(var_name)) == NULL)
        {
          v = add_var(var_name, 0);

          if (v == NULL)
            {
              FREE(var_name);

              return val;
            }
        }

      if (op == PLUS)
        v->value++;
      else
        v->value--;

      if (is_register(var_name))
        v->value = truncate_register(var_name, v->value);

      val = v->value;

      FREE(var_name);
    }
  else /* Normal unary operator */
    switch (op)
      {
        case NEGATIVE:
          val *= -(unsigned long long)1LL;
          break;

        case BANG:
          val = !val;
          break;

        case TWIDDLE:
          val = ~val;
          break;

        default:
          break;
      }

  return val;
}

static ULONG
get_value(char **str)
{
  ULONG val;
  char *var_name;
  variable *v;

  if (**str == SINGLE_QUOTE) /* A character constant */
    {
      unsigned int i;

      *str = *str + 1; /* Advance over the leading quote */
      val  = 0;
      for (i = 0; **str && **str != SINGLE_QUOTE && i < sizeof ( LONG );
           *str += 1, i++)
        {
          if (**str == '\\') /* Escape the next char */
            {
              *str += 1;

              if (!**str)
                {
                  (void)fprintf(stderr, "Invalid escape sequence.\n");

                  return 0;
                }
            }

          val <<= CHAR_BIT;
          val  |= (ULONG)((unsigned)**str );
        }

      if (**str != SINGLE_QUOTE) /* Constant must have been too long */
        {
          (void)fprintf(stderr, "Warning: character constant not terminated or too long (max len == %ld bytes)\n",
                        (long)sizeof ( LONG ));

          while (**str && **str != SINGLE_QUOTE)
            *str += 1;
        }
      else if (**str != '\0') //-V547
        *str += 1;
    }
  else if (isdigit((unsigned char)**str)) /* A regular number */
    {
      char *orig_str = *str; /* Store original for errors, etc. */
      errno = 0;
      val = xstrtoUL(orig_str, str, 0);

      if (errno)
        (void)fprintf(stderr, "Warning when converting input '%.*s': %s\n",
                      /*LINTED: E_CAST_INT_TO_SMALL_INT, E_PTRDIFF_OVERFLOW*/
                      (int)((ptrdiff_t)(*str - orig_str)), orig_str, xstrerror_l(errno));

      *str = skipwhite(*str);
    }
  else if (**str == USE_LAST_RESULT) /* '.' meaning use the last result */
    {
      val  = last_result;
      *str = skipwhite(*str + 1);
    }
  else if (**str == LPAREN) /* A parenthesized expression */
    {
      *str = skipwhite(*str + 1);
      val  = assignment_expr(str); /* Start at top and come back down */

      if (**str == RPAREN)
        *str = *str + 1;
      else
        (void)fprintf(stderr, "Mismatched paren's\n");
    }
  else if (isalpha((unsigned char)**str) || **str == '_') /* A variable name */
    {
      if (( var_name = get_var_name(str)) == NULL)
        {
          (void)fprintf(stderr, "Can't get var name!\n");

          return 0;
        }

      if (get_var(var_name, &val) == 0)
        {
          (void)fprintf(stderr, "No such variable: %s (assigning value of zero)\n",
                        var_name);

          val = 0;
          v   = add_var(var_name, val);

          if (v == NULL)
            return 0;
        }

      *str = skipwhite(*str);
      if (*str != NULL && (strncmp(*str, "++", 2) == 0 || strncmp(*str, "--", 2) == 0))
        {
          if ((v = lookup_var(var_name)) != NULL)
            {
              if (**str == '+')
                v->value++;
              else
                v->value--;

              if (is_register(var_name))
                v->value = truncate_register(var_name, v->value);

              val = v->value;

              *str = *str + 2;
            }
          else
            (void)fprintf(stderr, "%s is a read-only variable\n", var_name);
        }

      FREE(var_name);
    }
  else
    {
      (void)fprintf(stderr, "Expecting left paren, unary op, constant or variable.");
      (void)fprintf(stderr, "  Got: '%s'\n", *str);

      return 0;
    }

  return val;
}

/* vim: set ts=2 sw=2 tw=0 ai expandtab : */
