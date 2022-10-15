/*
 * Haiku PC -- programmer's calculator
 * vim: set ts=2:sw=2:tw=0:ai:expandtab
 * SPDX-License-Identifier: MIT
 * scspell-id: df6bd67e-4b7b-11ed-acad-80ee73e9b8e7
 * Source: https://git.haiku-os.org/haiku/tree/src/bin/pc/
 * Version: 2019-07-21
 */

/*
 * Copyright (c) 1993 Dominic Giampaolo <dbg@be.com>
 * Copyright (c) 1994 Joel Tesler <joel@engr.sgi.com>
 * Copyright (c) 2005 Axel Dörfler <axeld@pinc-software.de>
 * Copyright (c) 2005 Ingo Weinhold <ingo_weinhold@gmx.de>
 * Copyright (c) 2009 Oliver Tappe <zooey@hirschkaefer.de>
 * Copyright (c) 2017 Tuan Kiet Ho <tuankiet65@gmail.com>
 * Copyright (c) 2019 Adrien Destugues <pulkomandy@pulkomandy.tk>
 * Copyright (c) 2022 Jeffrey H. Johnson <trnsz@pobox.com>
 * Copyright (c) 2022 The DPS8M Development Team
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
