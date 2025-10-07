#/* \
set -x;PID=$$;p=$0;cc=$(command -v c99 2>/dev/null||command -v gcc 2>/dev/null||command -v clang 2>/dev/null||printf '%s\n' cc); if "$cc" -o "$p.out.$PID" "$p";then case "${p:?}" in *"/"*) dir=${0%"/"*} ;; *) dir=. ;; esac;PATH="$dir:$PATH";"$p.out.$PID" "$@";rm -f "$p.out.$PID";exit 0;else printf '%s\n' 'ERROR: Compilation failed' >&2;exit 1;fi
#*/

/*
 * pc: programmer's calculator (version 2025-10-07)
 * vim: set ts=2:sw=2:tw=0:ai:expandtab
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
 * supported.  It also handles parens and nested expresions as well as
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
 * printf string).
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
 * rules (i.e. alpha or underscore followed by alpha-numeric and
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

#define USE_XSTRTOULL

#if defined(__MVS__)
# undef _ALL_SOURCE
# define _ALL_SOURCE
#endif

#if defined (_POSIX_C_SOURCE)
# undef _POSIX_C_SOURCE
#endif

#define _POSIX_C_SOURCE 200809L

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

#include <ctype.h>   /* for isalnum, isalpha, isdigit, isprint, isspace ... */
#include <errno.h>   /* for errno ...                                       */
#include <limits.h>  /* for LONG_MIN, ULONG_MAX ...                         */
#include <stdio.h>   /* for fprintf, NULL, printf, stderr, fgets, stdin ... */
#include <stdlib.h>  /* for free, malloc, exit, abort, rand, realloc,       */
                     /*     strtoull ...                                    */
#include <string.h>  /* for strncmp, strlen, strcmp, strdup, strncat ...    */
#include <time.h>    /* for time ...                                        */
#include <unistd.h>  /* for getpid, getuid, getgid ...                      */

#if defined(_CH_)
# if !defined(USE_XSTRTOULL)
#  define USE_XSTRTOULL
# endif
#endif

#if defined(__MVS__) && !defined(__clang_version__)
# undef inline
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

/*
 * You should #define USE_LONG_LONG if your compiler supports the long long
 * data type, has strtoull(), and a %lld conversion specifier for printf.
 * Otherwise just comment out the #define and pc will use plain longs.
 */

#define USE_LONG_LONG

#if defined(USE_XSTRTOULL)
static unsigned long long
xstrtoull (const char *nptr, char **endptr, int base)
{
  const char *p = nptr;
  unsigned long long result = 0;
  int any = 0;
  int neg = 0;

  if ((base != 0 && (base < 2 || base > 36)))
    {
      if (endptr)
        *endptr = (char *)nptr;

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
                  p += 2;
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
                  p += 2;
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
                  p += 2;
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
                  p += 2;
                }
              else
                base = 0; //-V1048
            }

          if (base == 0)
            {
              base = 8;
            }
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

      if (result > ULLONG_MAX / (unsigned long long)base ||
         (result == ULLONG_MAX / (unsigned long long)base &&
         (unsigned long long)d > ULLONG_MAX % (unsigned long long)base))
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
            *endptr = (char *)p;

          return ULLONG_MAX;
        }

      result = result * (unsigned long long)base + (unsigned long long)d;
    }

  if (!any)
    {
      if (endptr)
        *endptr = (char *)nptr;

      return 0;
    }

  if (neg)
    result = (unsigned long long)(-(long long)result);

  if (endptr)
    *endptr = (char *)p;

  return result;
}
#endif

#ifdef USE_LONG_LONG
# define LONG    long long
# define ULONG   unsigned long long
# if defined(USE_XSTRTOULL)
#  define STRTOUL xstrtoull
# else
#  define STRTOUL strtoull
# endif
# define STR1    "%20llu  0x%.16llx signed: %20lld"
# define STR2    "%20llu  0x%.16llx"
# define STR3    " char: "
# define STR4    "%20lu  0x%-16.8lx signed: %20ld"
# define STR5    "%20lu  0x%-16.8lx"
#else  /* ifdef USE_LONG_LONG */
# define LONG    long
# define ULONG   unsigned long
# define STRTOUL strtoul
# define STR1    "%10lu\t 0x%.8lx\t signed: %10ld"
# define STR2    "%10lu\t 0x%.8lx"
# define STR3    " char: "
# define STR4    STR1
# define STR5    STR2
#endif /* ifdef USE_LONG_LONG */

static ULONG do_assignment_operator(char **str, char *var_name);
static ULONG parse_expression(char *str);    /* Top-level interface to parser */
static ULONG assignment_expr(char **str);    /* Assignments =, +=, *=, etc */
static ULONG logical_or_expr(char **str);    /* Logical OR  `||' */
static ULONG logical_and_expr(char **str);   /* Logical AND  `&&' */
static ULONG or_expr(char **str);            /* OR  `|' */
static ULONG xor_expr(char **str);           /* XOR `^' */
static ULONG and_expr(char **str);           /* AND `&' */
static ULONG equality_expr(char **str);      /* Equality ==, != */
static ULONG relational_expr(char **str);    /* Relational <, >, <=, >= */
static ULONG shift_expr(char **str);         /* Shifts <<, >> */
static ULONG add_expression(char **str);     /* Addition/Subtraction +, - */
static ULONG term(char **str);               /* Multiplication/Division *,%,/ */
static ULONG factor(char **str);             /* Negation, Logical NOT ~, ! */
static ULONG get_value(char **str);

static int get_var(char *name, ULONG *val);  /* External interfaces to vars */
#ifdef EXTERNAL
static void set_var(char *name, ULONG val);
#endif /* ifdef EXTERNAL */

static void do_input(void);         /* Reads stdin and calls parser */
static char *skipwhite(char *str);  /* Skip over input white space */

/*
 * Variables are kept in a simple singly-linked list. Not high
 * performance, but it's also an extremely small implementation.
 *
 * New variables get added to the head of the list.  Variables are
 * never deleted, though it wouldn't be hard to do that.
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

static variable *lookup_var(char *name);
static variable *add_var(char *name, ULONG value);

static char *get_var_name(char **input_str);
static void parse_args(int argc, char *argv[]);

static int(
  *set_var_lookup_hook(
    int ( *func ) (
      char *name,
      ULONG *val))) (
  char *name,
  ULONG *val);

/*
 * last_result is equal to the result of the last expression and
 * expressions can refer to it as `.' (just like bc).
 */

static ULONG last_result = 0;

static int
special_vars(char *name, ULONG *val)
{
  if (strcmp(name, "time") == 0)
    {
      *val = (ULONG)time(NULL);
    }
  else if (strcmp(name, "rand") == 0)
    {
      *val = (ULONG)rand();
    }
  else if (strcmp(name, "dbg") == 0)
    {
      *val = 0x82969;
    }
  else if (strcmp(name, "pid") == 0)
    {
      *val = (ULONG)getpid();
    }
  else if (strcmp(name, "uid") == 0)
    {
      *val = (ULONG)getuid();
    }
  else if (strcmp(name, "gid") == 0)
    {
      *val = (ULONG)getgid();
    }
  else if (strcmp(name, "errno") == 0)
    {
      *val = (ULONG)errno;
    }
  else if (strcmp(name, "ULLONG_MAX") == 0)
    {
      *val = (ULONG)ULLONG_MAX;
    }
  else if (strcmp(name, "LLONG_MAX") == 0)
    {
      *val = (ULONG)LLONG_MAX;
    }
  else if (strcmp(name, "LLONG_MIN") == 0)
    {
      *val = (ULONG)LLONG_MIN;
    }
  else if (strcmp(name, "LONG_MAX") == 0)
    {
      *val = (ULONG)LONG_MAX;
    }
  else if (strcmp(name, "LONG_MIN") == 0)
    {
      *val = (ULONG)LONG_MIN;
    }
  else if (strcmp(name, "INT_MAX") == 0)
    {
      *val = (ULONG)INT_MAX;
    }
  else if (strcmp(name, "INT_MIN") == 0)
    {
      *val = (ULONG)INT_MIN;
    }
  else if (strcmp(name, "ARG_MAX") == 0)
    {
      *val = (ULONG)sysconf(_SC_ARG_MAX);
    }
  else if (strcmp(name, "CHILD_MAX") == 0)
    {
      *val = (ULONG)sysconf(_SC_CHILD_MAX);
    }
  else if (strcmp(name, "OPEN_MAX") == 0)
    {
      *val = (ULONG)sysconf(_SC_OPEN_MAX);
    }
  else if (strcmp(name, "PATH_MAX") == 0)
    {
      *val = (ULONG)pathconf("/", _PC_PATH_MAX);
    }
  else if (strcmp(name, "NAME_MAX") == 0)
    {
      *val = (ULONG)pathconf(".", _PC_NAME_MAX);
    }
  else if (strcmp(name, "CHAR_BIT") == 0)
    {
      *val = (ULONG)CHAR_BIT;
    }
  else if (strcmp(name, "CHAR_MAX") == 0)
    {
      *val = (ULONG)CHAR_MAX;
    }
  else if (strcmp(name, "CHAR_MIN") == 0)
    {
      *val = (ULONG)CHAR_MIN;
    }
  else if (strcmp(name, "SCHAR_MAX") == 0)
    {
      *val = (ULONG)SCHAR_MAX;
    }
  else if (strcmp(name, "SCHAR_MIN") == 0)
    {
      *val = (ULONG)SCHAR_MIN;
    }
  else if (strcmp(name, "UCHAR_MAX") == 0)
    {
      *val = (ULONG)UCHAR_MAX;
    }
  else if (strcmp(name, "SHRT_MAX") == 0)
    {
      *val = (ULONG)SHRT_MAX;
    }
  else if (strcmp(name, "SHRT_MIN") == 0)
    {
      *val = (ULONG)SHRT_MIN;
    }
  else if (strcmp(name, "USHRT_MAX") == 0)
    {
      *val = (ULONG)USHRT_MAX;
    }
  else if (strcmp(name, "UINT_MAX") == 0)
    {
      *val = (ULONG)UINT_MAX;
    }
  else if (strcmp(name, "ULONG_MAX") == 0)
    {
      *val = (ULONG)ULONG_MAX;
    }
  else if (strcmp(name, "EOF") == 0)
    {
      *val = (ULONG)EOF;
    }
  else if (strcmp(name, "STDIN_FILENO") == 0)
    {
      *val = (ULONG)STDIN_FILENO;
    }
  else if (strcmp(name, "STDOUT_FILENO") == 0)
    {
      *val = (ULONG)STDOUT_FILENO;
    }
  else if (strcmp(name, "STDERR_FILENO") == 0)
    {
      *val = (ULONG)STDERR_FILENO;
    }
  else if (strcmp(name, "sizeof_char") == 0)
    {
      *val = (ULONG)sizeof(char);
    }
  else if (strcmp(name, "sizeof_short") == 0)
    {
      *val = (ULONG)sizeof(short);
    }
  else if (strcmp(name, "sizeof_int") == 0)
    {
      *val = (ULONG)sizeof(int);
    }
  else if (strcmp(name, "sizeof_long") == 0)
    {
      *val = (ULONG)sizeof(long);
    }
  else if (strcmp(name, "sizeof_ll") == 0)
    {
      *val = (ULONG)sizeof(long long);
    }
  else if (strcmp(name, "sizeof_void") == 0)
    {
      *val = (ULONG)sizeof(void *);
    }
  else
    {
      return 0;
    }

  return 1;
}

int
main(int argc, char *argv[])
{
  (void)set_var_lookup_hook(special_vars);

  if (argc > 1)
    {
      parse_args(argc, argv);
    }
  else
    {
      do_input();
    }

  return 0;
}

/*
 * This function prints the result of the expression.
 * It tries to be smart about printing numbers so it
 * only uses the necessary number of digits.  If you
 * have long long (i.e. 64 bit numbers) it's very
 * annoying to have lots of leading zeros when they
 * aren't necessary.  By doing the somewhat bizarre
 * casting and comparisons we can determine if a value
 * will fit in a 32 bit quantity and only print that.
 */

static void
print_result(ULONG value)
{
  int i;
  ULONG ch, shift;

  if ((signed LONG)value < 0)
    {
      if ((signed LONG)value < (signed LONG)( LONG_MIN )) //-V547
        {
          (void)printf(STR1, value, value, value);
        }
      else
        {
          (void)printf(STR4, (unsigned long)value, (long)value, (long)value);
        }
    }
  else if ((ULONG)value <= (ULONG)ULONG_MAX) //-V547
    {
      (void)printf(STR5, (unsigned long)value, (long)value);
    }
  else
    {
      (void)printf(STR2, value, value);
    }

  /*
   * Print any printable character (and print dots for unprintable chars)
   */

  (void)printf(STR3);
  for (i = sizeof ( ULONG ) - 1; i >= 0; i--)
    {
      shift = (ULONG)i * (long long)CHAR_BIT;
      ch = ((ULONG)value & ((ULONG)0xff << shift )) >> shift;

      if (isprint((int)ch))
        {
          (void)printf("%c", (char)( ch ));
        }
      else
        {
          (void)printf(".");
        }
    }

  (void)printf("\n");
}

static void
parse_args(int argc, char *argv[])
{
  int i, len;
  char *buff;
  ULONG value;

  for (i = 1, len = 0; i < argc; i++)
    {
      len += strlen(argv[i]) + 1;
    }

  len++;

  buff = malloc((ULONG)len * sizeof ( char ));
  if (buff == NULL)
    {
      return;
    }

  buff[0] = '\0';
  for (i = 1; i < argc; i++)
    {
      (void)strncat(buff, argv[i], len - strlen(buff) - 1);
      (void)strncat(buff, " ", len - strlen(buff) - 1);
    }
  value = parse_expression(buff);

  print_result(value);

  free(buff);
  buff = NULL;
}

static void
do_input(void)
{
  ULONG value;
  char buff[256], *ptr;

  while (fgets(buff, 256, stdin) != NULL)
    {
      if (strlen(buff) >= 255)
        {
          (void)fprintf(stderr, "FATAL: Oversize input!\n");
          exit(1);
        }

      if (buff[0] != '\0' && buff[strlen(buff) - 1] == '\n') //-V557
        {
          buff[strlen(buff) - 1] = '\0'; //-V557 /* Kill the newline character */
        }

      for (ptr = buff; isspace(*ptr) && *ptr; ptr++)
        { /* Skip whitespace */
        }

      if (*ptr == '\0') /* Hmmm, an empty line, just skip it */
        {
          continue;
        }

      value = parse_expression(buff);

      print_result(value);
    }
}

static ULONG
parse_expression(char *str)
{
  ULONG val;
  char *ptr = str;

  ptr = skipwhite(ptr);
  if (*ptr == '\0')
    {
      return last_result;
    }

  val = assignment_expr(&ptr);
  ptr = skipwhite(ptr);
  while (*ptr == SEMI_COLON /* && *ptr != '\0' */)
    {
      ptr++;
      if (*ptr == '\0') /* Reached the end of the string, stop parsing */
        {
          continue;
        }

      val = assignment_expr(&ptr);
    }

  last_result = val;
  return val;
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
  *str     = skipwhite(*str);
  if (**str == EQUAL && *( *str + 1 ) != EQUAL)
    {
      *str = skipwhite(*str + 1);  /* Skip the equal sign */
      val  = assignment_expr(str); /* Go recursive! */

      if (( v = lookup_var(var_name)) == NULL)
        {
          (void)add_var(var_name, val);
        }
      else
        {
          v->value = val;
        }
    }
  else if ((( **str == PLUS || **str == MINUS || **str == OR || **str == TIMES
              || **str == DIVISION || **str == MODULO || **str == AND
              || **str == XOR )
            && *( *str + 1 ) == EQUAL )
           || strncmp(*str, "<<=", 3) == 0 || strncmp(*str, ">>=", 3) == 0)
    {
      val = do_assignment_operator(str, var_name);
    }
  else
    {
      *str = orig_str;
      val  = logical_or_expr(str); /* No equal sign, just get var value */
      *str = skipwhite(*str);

      if (*str == NULL)
        {
          (void)fprintf(stderr, "FATAL: Bugcheck: str == NULL %s[%s:%d]\n",
                  __FILE__, __func__, __LINE__);
          abort();
        }

      if (**str == EQUAL)
        {
          (void)fprintf(
            stderr,
            "Left hand side of expression is not assignable.\n");
        }
    }

  if (var_name)
    {
      free(var_name);
      var_name = NULL;
    }

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
    {
      *str = skipwhite(*str + 3);
    }
  else
    {
      *str = skipwhite(*str + 2); /* Skip the assignment operator */
    }

  val = assignment_expr(str);  /* Go recursive! */
  v   = lookup_var(var_name);
  if (v == NULL)
    {
      v = add_var(var_name, 0);
      if (v == NULL)
        {
          return 0;
        }
    }

  if (operator == PLUS)
    {
      if (v->value > (ULONG)-1 - val)
        {
          errno = ERANGE;
          (void)fprintf(stderr, "Warning: %s (Overflow)\n", strerror(errno));
        }
      v->value += val;
    }
  else if (operator == MINUS)
    {
      if (val > v->value)
        {
          errno = ERANGE;
          (void)fprintf(stderr, "Warning: %s (Underflow)\n", strerror(errno));
        }
      v->value -= val;
    }
  else if (operator == AND)
    {
      v->value &= val;
    }
  else if (operator == XOR)
    {
      v->value ^= val;
    }
  else if (operator == OR)
    {
      v->value |= val;
    }
  else if (operator == SHIFT_L)
    {
      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", strerror(errno));
        }
      v->value <<= val;
    }
  else if (operator == SHIFT_R)
    {
      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", strerror(errno));
        }
      v->value >>= val;
    }
  else if (operator == TIMES)
    {
      if (val != 0 && v->value > (ULONG)-1 / val)
        {
          errno = ERANGE;
          (void)fprintf(stderr, "Warning: %s (Overflow)\n", strerror(errno));
        }
      v->value *= val;
    }
  else if (operator == DIVISION)
    {
      if (val == 0) /* Check for it, but still get the result */
        {
          errno = EDOM;
          (void)fprintf(stderr, "Warning: %s (Division by zero)\n", strerror(errno));
          v->value = 0;
        }
      else
        {
          v->value /= val;
        }
    }
  else if (operator == MODULO)
    {
      if (val == 0) /* check for it, but still get the result */
        {
          errno = EDOM;
          (void)fprintf(stderr, "Warning: %s (Modulo by zero)\n", strerror(errno));
          v->value = 0;
        }
      else
        {
          v->value %= val;
        }
    }
  else
    {
      (void)fprintf(stderr, "Unknown operator: %c\n", operator);
      v->value = 0;
    }

  return v->value;
}

static ULONG
logical_or_expr(char **str)
{
  ULONG val, sum = 0;

  *str = skipwhite(*str);
  sum  = logical_and_expr(str);
  *str = skipwhite(*str);
  while (**str == OR && *( *str + 1 ) == OR)
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
  while (**str == AND && *( *str + 1 ) == AND)
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
  while (**str == OR && *( *str + 1 ) != OR)
    {
      *str  = skipwhite(*str + 1); /* Advance over the operator */
      val   = xor_expr(str);
      sum  |= val;
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
  while (**str == XOR)
    {
      *str  = skipwhite(*str + 1); /* Advance over the operator */
      val   = and_expr(str);
      sum  ^= val;
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
  while (**str == AND && *( *str + 1 ) != AND)
    {
      *str  = skipwhite(*str + 1); /* Advance over the operator */
      val   = equality_expr(str);
      sum  &= val;
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
  while (( **str == EQUAL && *( *str + 1 ) == EQUAL )
         || ( **str == BANG && *( *str + 1 ) == EQUAL ))
    {
      op   = **str;
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = relational_expr(str);

      if (op == EQUAL)
        {
          sum = ( sum == val );
        }
      else if (op == BANG) //-V547
        {
          sum = ( sum != val );
        }
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
  while (**str == LESS_THAN || **str == GREATER_THAN)
    {
      equal_to = 0;
      op       = **str;

      if (*( *str + 1 ) == EQUAL)
        {
          equal_to = 1;
          *str = *str + 1; /* Skip initial operator */
        }

      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = shift_expr(str);

      /*
       * Notice that we do the relational expressions as signed
       * comparisons.  This is because of expressions like:
       *      0 > -1
       * which would not return the expected value if we did the
       * comparison as unsigned.  This may not always be the
       * desired behavior, but aside from adding casting to epxressions,
       * there isn't much of a way around it.
       */

      if (op == LESS_THAN && equal_to == 0)
        {
          sum = ((LONG)sum < (LONG)val );
        }
      else if (op == LESS_THAN && equal_to == 1)
        {
          sum = ((LONG)sum <= (LONG)val );
        }
      else if (op == GREATER_THAN && equal_to == 0)
        {
          sum = ((LONG)sum > (LONG)val );
        }
      else if (op == GREATER_THAN && equal_to == 1)
        {
          sum = ((LONG)sum >= (LONG)val );
        }
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
  while (( strncmp(*str, "<<", 2) == 0 ) || ( strncmp(*str, ">>", 2) == 0 ))
    {
      op   = **str;
      *str = skipwhite(*str + 2); /* Advance over the operator */
      val  = add_expression(str);

      if (val >= sizeof(ULONG) * CHAR_BIT)
        {
          errno = EINVAL;
          (void)fprintf(stderr, "Warning: %s (Shift too many bits)\n", strerror(errno));
        }

      if (op == SHIFT_L)
        {
          sum <<= val;
        }
      else if (op == SHIFT_R)
        {
          sum >>= val;
        }
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
  while (**str == PLUS || **str == MINUS)
    {
      op   = **str;
      *str = skipwhite(*str + 1); /* Advance over the operator */
      val  = term(str);

      if (op == PLUS)
        {
          if (sum > (ULONG)-1 - val)
            {
              errno = ERANGE;
              (void)fprintf(stderr, "Warning: %s (Overflow)\n", strerror(errno));
            }
          sum += val;
        }
      else if (op == MINUS) //-V547
        {
          if (val > sum)
            {
              errno = ERANGE;
              (void)fprintf(stderr, "Warning: %s (Underflow)\n", strerror(errno));
            }
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

  while (**str == TIMES || **str == DIVISION || **str == MODULO)
    {
      op   = **str;
      *str = skipwhite(*str + 1);
      val  = factor(str);

      if (op == TIMES)
        {
          if (val != 0 && sum > (ULONG)-1 / val)
            {
              errno = ERANGE;
              (void)fprintf(stderr, "Warning: %s (Overflow)\n", strerror(errno));
            }
          sum *= val;
        }
      else if (op == DIVISION)
        {
          if (val == 0)
            {
              errno = EDOM;
              (void)fprintf(stderr, "Warning: %s (Division by zero)\n", strerror(errno));
              sum = 0;
            }
          else
            {
              sum /= val;
            }
        }
      else if (op == MODULO) //-V547
        {
          if (val == 0)
            {
              errno = EDOM;
              (void)fprintf(stderr, "Warning: %s (Modulo by zero)\n", strerror(errno));
              sum = 0;
            }
          else
            {
              sum %= val;
            }
        }
    }

  /*
   * We're at the bottom of the parse.  At this point we either have
   * an operator or we're through with this string.  Otherwise it's
   * an error and we print a message.
   */

  if (**str != TIMES && **str != DIVISION && **str != MODULO && **str != PLUS //-V560
      && **str != MINUS && **str != OR && **str != AND && **str != XOR
      && **str != BANG && **str != NEGATIVE && **str != TWIDDLE //-V560
      && **str != RPAREN && **str != LESS_THAN && **str != GREATER_THAN
      && **str != SEMI_COLON && strncmp(*str, "<<", 2) != 0
      && strncmp(*str, ">>", 2) && **str != EQUAL && **str != '\0') //-V526
    {
      (void)fprintf(stderr, "Parsing stopped: unknown operator %s\n", *str);
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

      if (( op == NEGATIVE && *( *str + 1 ) == NEGATIVE ) || /* Look for ++/-- */
          ( op == PLUS && *( *str + 1 ) == PLUS ))
        {
          *str         = *str + 1;
          have_special = 1;
        }

      *str         = skipwhite(*str + 1);
      var_name_ptr = *str; /* Save where the varname should be */
    }

  val  = get_value(str);
  *str = skipwhite(*str);

  /*
   * Now is the time to actually do the unary operation if one
   * was present.
   */

  if (have_special) /* We've got a ++ or -- */
    {
      var_name = get_var_name(&var_name_ptr);
      if (var_name == NULL)
        {
          (void)fprintf(stderr, "Can only use ++/-- on variables.\n");
          return val;
        }

      if (( v = lookup_var(var_name)) == NULL)
        {
          v = add_var(var_name, 0);
          if (v == NULL)
            {
              return val;
            }
        }

      free(var_name);
      var_name = NULL;

      if (op == PLUS)
        {
          val = ++v->value;
        }
      else
        {
          val = --v->value;
        }
    }
  else /* Normal unary operator */
    {
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
        }
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
          (void)fprintf(
            stderr,
            "Warning: character constant not terminated or too "
            "long (max len == %ld bytes)\n",
            sizeof ( LONG ));
          while (**str && **str != SINGLE_QUOTE)
            {
              *str += 1;
            }
        }
      else if (**str != '\0') //-V547
        {
          *str += 1;
        }
    }
  else if (isdigit(**str))  /* A regular number */
    {
      errno = 0;
      val  = STRTOUL(*str, str, 0);
      if (errno)
        {
          (void)fprintf(stderr, "Warning: %s\n", strerror(errno));
        }
      *str = skipwhite(*str);
    }
  else if (**str == USE_LAST_RESULT) /* A `.' meaning use the last result */
    {
      val  = last_result;
      *str = skipwhite(*str + 1);
    }
  else if (**str == LPAREN) /* A parenthesized expression */
    {
      *str = skipwhite(*str + 1);
      val  = assignment_expr(str); /* Start at top and come back down */

      if (**str == RPAREN)
        {
          *str = *str + 1;
        }
      else
        {
          (void)fprintf(stderr, "Mismatched paren's\n");
        }
    }
  else if (isalpha(**str) || **str == '_')  /* A variable name */
    {
      if (( var_name = get_var_name(str)) == NULL)
        {
          (void)fprintf(stderr, "Can't get var name!\n");
          return 0;
        }

      if (get_var(var_name, &val) == 0)
        {
          (void)fprintf(
            stderr,
            "No such variable: %s (assigning value of zero)\n",
            var_name);

          val = 0;
          v   = add_var(var_name, val);
          if (v == NULL)
            {
              return 0;
            }
        }

      *str = skipwhite(*str);
      if (strncmp(*str, "++", 2) == 0 || strncmp(*str, "--", 2) == 0)
        {
          if (( v = lookup_var(var_name)) != NULL)
            {
              val = v->value;
              if (**str == '+')
                {
                  v->value++;
                }
              else
                {
                  v->value--;
                }

              *str = *str + 2;
            }
          else
            {
              (void)fprintf(stderr, "%s is a read-only variable\n", var_name);
            }
        }

      free(var_name);
      var_name = NULL;
    }
  else
    {
      (void)fprintf(
        stderr,
        "Expecting left paren, unary op, constant or variable.");
      (void)fprintf(stderr, "  Got: `%s'\n", *str);
      return 0;
    }

  return val;
}

/*
 * Here are the functions that manipulate variables.
 */

/*
 * This is a hook function for external read-only
 * variables.  If it is set and we don't find a
 * variable name in our name space, we call it to
 * look for the variable.  If it finds the name, it
 * fills in val and returns 1. If it returns 0, it
 * didn't find the variable.
 */

static int (*external_var_lookup) (char *name, ULONG *val) = NULL;

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
      char *name,
      ULONG *val))) (
  char *name,
  ULONG *val)
{
  int (*old_func) (char *name, ULONG *val) = external_var_lookup;

  external_var_lookup = func;

  return old_func;
}

static inline variable *
lookup_var(char *name)
{
  variable *v;

  for (v = vars; v; v = v->next)
    {
      if (v->name && strcmp(v->name, name) == 0)
        {
          return v;
        }
    }

  return NULL;
}

static variable *
add_var(char *name, ULONG value)
{
  variable *v;
  ULONG tmp;

  /* First make sure this isn't an external read-only variable */

  if (external_var_lookup)
    {
      if (external_var_lookup(name, &tmp) != 0)
        {
          (void)fprintf(
            stderr,
            "Can't assign/create %s, it is a read-only var\n",
            name);
          return NULL;
        }
    }

  v = (variable *)malloc(sizeof ( variable ));
  if (v == NULL)
    {
      (void)fprintf(stderr, "No memory to add variable: %s\n", name);
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

#ifdef EXTERNAL
static void
set_var(char *name, ULONG val)
{
  variable *v;

  v = lookup_var(name);
  if (v != NULL)
    {
      v->value = val;
    }
  else
    {
      (void)add_var(name, val);
    }
}
#endif /* ifdef EXTERNAL */

/*
 * This function returns 1 on success of finding
 * a variable and 0 on failure to find a variable.
 * If a variable is found, val is filled with its
 * value.
 */

static int
get_var(char *name, ULONG *val)
{
  variable *v;

  v = lookup_var(name);
  if (v != NULL)
    {
      *val = v->value;
      return 1;
    }
  else if (external_var_lookup != NULL)
    {
      return external_var_lookup(name, val);
    }

  return 0;
}

#define DEFAULT_LEN 32

static char *
get_var_name(char **str)
{
  int i, len = DEFAULT_LEN;
  char *buff, *tmpbuff;

  if (isalpha(**str) == 0 && **str != '_')
    {
      return NULL;
    }

  buff = (char *)malloc((ULONG)len * sizeof ( char ));
  if (buff == NULL)
    {
      return NULL;
    }

  /*
   * First get the variable name
   */

  i = 0;
  while (**str && ( isalnum(**str) || **str == '_' ))
    {
      if (i >= len - 1)
        {
          len     *= 2;
          tmpbuff  = (char *)realloc(buff, (ULONG)len);
          if (tmpbuff == NULL)
            {
              return NULL;
            }

          buff = tmpbuff;
        }

      buff[i++] = **str;
      *str      = *str + 1;
    }

  buff[i] = '\0'; /* NULL terminate */

  while (isalnum(**str) || **str == '_')  /* Skip over any remaining junk */
    {
      *str = *str + 1;
    }

  return buff;
}

static char *
skipwhite(char *str)
{
  if (str == NULL)
    {
      return NULL;
    }

  while (*str && ( *str == ' ' || *str == '\t' || *str == '\n' || *str == '\f' ))
    {
      str++;
    }

  return str;
}
