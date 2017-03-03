
/*  A Bison parser, made from minicompg.y
 by  GNU Bison version 1.27
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	TKNUMBER	257
#define	TKIDENTIFIER	258
#define	TKREAL	259
#define	TKINT	260
#define	TKLEQ	261
#define	TKGEQ	262
#define	TKNEQ	263
#define	TKIF	264
#define	TKELSE	265
#define	TKWHILE	266
#define	TKRETURN	267
#define	TKASSIGN	268
#define	TKAND	269
#define	TKOR	270

#line 18 "minicompg.y"

#include "minicomp.h"

typedef void *voidptr;
#define YYSTYPE voidptr
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		123
#define	YYFLAG		-32768
#define	YYNTBASE	32

#define YYTRANSLATE(x) ((unsigned)(x) <= 270 ? yytranslate[x] : 64)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    23,
    24,    27,    25,    29,    26,     2,    28,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    19,    21,
    20,    22,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    17,     2,    18,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    30,     2,    31,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     5,     7,     9,    11,    13,    18,    21,    23,
    27,    29,    33,    37,    39,    43,    47,    51,    55,    59,
    63,    67,    71,    75,    77,    81,    85,    87,    91,    93,
    98,   102,   104,   109,   111,   113,   117,   119,   122,   124,
   126,   128,   130,   132,   134,   135,   139,   143,   146,   149,
   151,   152,   160,   161,   162,   166,   167,   174,   179,   183,
   186,   190,   194,   197,   201,   203
};

static const short yyrhs[] = {    32,
    33,     0,    33,     0,    59,     0,    36,     0,     5,     0,
     6,     0,    34,    17,    40,    18,     0,    35,    36,     0,
    36,     0,    34,    37,    19,     0,     4,     0,    38,    15,
    39,     0,    38,    16,    39,     0,    39,     0,    40,    20,
    40,     0,    40,     9,    40,     0,    40,    21,    40,     0,
    40,    22,    40,     0,    40,     7,    40,     0,    40,     8,
    40,     0,    23,    38,    24,     0,    40,    25,    41,     0,
    40,    26,    41,     0,    41,     0,    41,    27,    42,     0,
    41,    28,    42,     0,    42,     0,    23,    40,    24,     0,
     3,     0,    44,    23,    45,    24,     0,    44,    23,    24,
     0,    43,     0,    42,    17,    40,    18,     0,    44,     0,
     4,     0,    45,    29,    40,     0,    40,     0,    46,    47,
     0,    47,     0,    51,     0,    55,     0,    57,     0,    58,
     0,    48,     0,     0,    30,    49,    50,     0,    35,    46,
    31,     0,    46,    31,     0,    35,    31,     0,    31,     0,
     0,    10,    23,    38,    24,    52,    48,    53,     0,     0,
     0,    11,    54,    48,     0,     0,    12,    23,    38,    24,
    56,    48,     0,    43,    14,    40,    19,     0,    13,    40,
    19,     0,    60,    61,     0,    34,    37,    23,     0,    62,
    24,    48,     0,    24,    48,     0,    62,    29,    63,     0,
    63,     0,    34,    37,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    27,    29,    32,    34,    37,    39,    40,    43,    45,    48,
    52,    56,    58,    59,    62,    64,    65,    66,    67,    68,
    69,    72,    73,    74,    77,    78,    79,    82,    84,    85,
    86,    87,    90,    92,    95,    99,   101,   104,   106,   109,
   111,   112,   113,   114,   117,   119,   121,   123,   124,   125,
   129,   130,   133,   135,   135,   139,   140,   143,   147,   151,
   155,   159,   161,   164,   166,   169
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TKNUMBER",
"TKIDENTIFIER","TKREAL","TKINT","TKLEQ","TKGEQ","TKNEQ","TKIF","TKELSE","TKWHILE",
"TKRETURN","TKASSIGN","TKAND","TKOR","'['","']'","';'","'='","'<'","'>'","'('",
"')'","'+'","'-'","'*'","'/'","','","'{'","'}'","glob_decl_list","glob_decl",
"type","decl_list","decl","decl_id","cond_expr","ao_expr","arith_expr","term",
"factor","lvalue","variable","arg_list","stmt_list","stmt","block","@1","block_end",
"if_stmt","@2","else_part","@3","while_stmt","@4","assgn_stmt","return_stmt",
"func_decl","func_decl_begin","func_decl_end","par_list","par_decl", NULL
};
#endif

static const short yyr1[] = {     0,
    32,    32,    33,    33,    34,    34,    34,    35,    35,    36,
    37,    38,    38,    38,    39,    39,    39,    39,    39,    39,
    39,    40,    40,    40,    41,    41,    41,    42,    42,    42,
    42,    42,    43,    43,    44,    45,    45,    46,    46,    47,
    47,    47,    47,    47,    49,    48,    50,    50,    50,    50,
    52,    51,    53,    54,    53,    56,    55,    57,    58,    59,
    60,    61,    61,    62,    62,    63
};

static const short yyr2[] = {     0,
     2,     1,     1,     1,     1,     1,     4,     2,     1,     3,
     1,     3,     3,     1,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     1,     3,     3,     1,     3,     1,     4,
     3,     1,     4,     1,     1,     3,     1,     2,     1,     1,
     1,     1,     1,     1,     0,     3,     3,     2,     2,     1,
     0,     7,     0,     0,     3,     0,     6,     4,     3,     2,
     3,     3,     2,     3,     1,     2
};

static const short yydefact[] = {     0,
     5,     6,     0,     2,     0,     4,     3,     0,     1,    11,
     0,     0,     0,     0,    60,     0,    65,    29,    35,     0,
     0,    24,    27,    32,    34,    10,    61,    45,    63,    66,
     0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
     0,    62,    64,    28,    22,    23,    25,    26,     0,    31,
    37,     0,     0,     0,     0,    50,     0,     0,     9,     0,
    32,     0,    39,    44,    46,    40,    41,    42,    43,    33,
    30,     0,     0,     0,     0,     0,    49,     8,     0,     0,
    48,    38,    36,     0,     0,    14,     0,     0,    59,    47,
     0,     0,     0,     0,     0,    51,     0,     0,     0,     0,
     0,     0,    56,    58,    21,    12,    13,     0,    19,    20,
    16,    15,    17,    18,     0,    53,    57,    54,    52,     0,
    55,     0,     0
};

static const short yydefgoto[] = {     3,
     4,     5,    58,     6,    12,    85,    86,    87,    22,    23,
    24,    25,    52,    62,    63,    64,    41,    65,    66,   108,
   119,   120,    67,   115,    68,    69,     7,     8,    15,    16,
    17
};

static const short yypact[] = {    33,
-32768,-32768,   148,-32768,    27,-32768,-32768,     0,-32768,-32768,
    50,    75,   -21,    27,-32768,   128,-32768,-32768,-32768,    50,
    25,   102,    -5,-32768,    32,-32768,-32768,-32768,-32768,-32768,
   -21,    33,   137,-32768,    50,    50,    50,    50,    50,    44,
    10,-32768,-32768,-32768,   102,   102,    -5,    -5,    46,-32768,
   133,   131,    78,    81,    50,-32768,    27,    53,-32768,    -5,
    55,    93,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    50,    57,    57,    56,    60,-32768,-32768,   105,    50,
-32768,-32768,   133,    57,    62,-32768,   125,    95,-32768,-32768,
    74,    97,   118,    57,    57,-32768,    50,    50,    50,    50,
    50,    50,-32768,-32768,-32768,-32768,-32768,   -21,   133,   133,
   133,   133,   133,   133,   -21,   103,-32768,-32768,-32768,   -21,
-32768,    85,-32768
};

static const short yypgoto[] = {-32768,
   117,    -6,-32768,   -24,   -11,   -47,    70,   -10,   132,   -30,
   -37,-32768,-32768,    64,   -43,   -13,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    99
};


#define	YYLAST		168


static const short yytable[] = {    29,
    21,    14,    30,    61,     1,     2,    47,    48,    28,    33,
    60,    39,    18,    19,     1,     2,    59,    42,    82,    53,
    61,    54,    55,    13,    61,    14,    88,    60,    49,    51,
    10,    60,    20,    78,    57,    82,    92,     1,     2,    28,
    56,    61,    34,    11,    75,    76,    18,    19,    60,    35,
    36,    57,    18,    19,    40,    18,    19,     1,     2,    18,
    19,    83,    53,    70,    54,    55,    20,    50,    80,    91,
    35,    36,    20,    93,    89,    20,    94,    95,    26,    84,
    35,    36,    28,    77,   123,    96,   109,   110,   111,   112,
   113,   114,   104,    26,   116,    18,    19,    27,    35,    36,
    73,   117,    53,    74,    54,    55,   121,    18,    19,    94,
    95,    94,    95,   118,    53,    20,    54,    55,   103,     9,
   105,    79,    28,    81,    97,    98,    99,    20,    37,    38,
    43,    97,    98,    99,    28,    90,     0,   100,   101,   102,
     0,    44,    35,    36,   100,   101,   102,   122,     0,    35,
    36,    31,     1,     2,    71,     0,    32,    35,    36,    72,
    44,    35,    36,   106,   107,     0,    45,    46
};

static const short yycheck[] = {    13,
    11,     8,    14,    41,     5,     6,    37,    38,    30,    20,
    41,    17,     3,     4,     5,     6,    41,    31,    62,    10,
    58,    12,    13,    24,    62,    32,    74,    58,    39,    40,
     4,    62,    23,    58,    41,    79,    84,     5,     6,    30,
    31,    79,    18,    17,    55,    57,     3,     4,    79,    25,
    26,    58,     3,     4,    23,     3,     4,     5,     6,     3,
     4,    72,    10,    18,    12,    13,    23,    24,    14,    80,
    25,    26,    23,    84,    19,    23,    15,    16,    19,    23,
    25,    26,    30,    31,     0,    24,    97,    98,    99,   100,
   101,   102,    19,    19,   108,     3,     4,    23,    25,    26,
    23,   115,    10,    23,    12,    13,   120,     3,     4,    15,
    16,    15,    16,    11,    10,    23,    12,    13,    24,     3,
    24,    58,    30,    31,     7,     8,     9,    23,    27,    28,
    32,     7,     8,     9,    30,    31,    -1,    20,    21,    22,
    -1,    24,    25,    26,    20,    21,    22,     0,    -1,    25,
    26,    24,     5,     6,    24,    -1,    29,    25,    26,    29,
    24,    25,    26,    94,    95,    -1,    35,    36
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.27.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 216 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 5:
#line 38 "minicompg.y"
{yyval=new_type(DOUBLE);;
    break;}
case 6:
#line 39 "minicompg.y"
{yyval=new_type(INT);;
    break;}
case 7:
#line 40 "minicompg.y"
{yyval=new_array(yyvsp[-3],yyvsp[-1]);;
    break;}
case 10:
#line 49 "minicompg.y"
{add_var(yyvsp[-1],yyvsp[-2]);;
    break;}
case 11:
#line 53 "minicompg.y"
{yyval=add_string(tkname);;
    break;}
case 12:
#line 57 "minicompg.y"
{yyval=binary_node(NAND,yyvsp[-2],yyvsp[0]);;
    break;}
case 13:
#line 58 "minicompg.y"
{yyval=binary_node(NOR,yyvsp[-2],yyvsp[0]);;
    break;}
case 15:
#line 63 "minicompg.y"
{yyval=binary_node(NEQUALS,yyvsp[-2],yyvsp[0]);;
    break;}
case 16:
#line 64 "minicompg.y"
{yyval=binary_node(NNEQ,yyvsp[-2],yyvsp[0]);;
    break;}
case 17:
#line 65 "minicompg.y"
{yyval=binary_node(NLT,yyvsp[-2],yyvsp[0]);;
    break;}
case 18:
#line 66 "minicompg.y"
{yyval=binary_node(NGT,yyvsp[-2],yyvsp[0]);;
    break;}
case 19:
#line 67 "minicompg.y"
{yyval=binary_node(NLEQ,yyvsp[-2],yyvsp[0]);;
    break;}
case 20:
#line 68 "minicompg.y"
{yyval=binary_node(NGEQ,yyvsp[-2],yyvsp[0]);;
    break;}
case 21:
#line 69 "minicompg.y"
{yyval=yyvsp[-1];
    break;}
case 22:
#line 72 "minicompg.y"
{yyval=binary_node(NADD,yyvsp[-2],yyvsp[0]);;
    break;}
case 23:
#line 73 "minicompg.y"
{yyval=binary_node(NSUB,yyvsp[-2],yyvsp[0]);;
    break;}
case 25:
#line 77 "minicompg.y"
{yyval=binary_node(NMUL,yyvsp[-2],yyvsp[0]);;
    break;}
case 26:
#line 78 "minicompg.y"
{yyval=binary_node(NDIV,yyvsp[-2],yyvsp[0]);;
    break;}
case 28:
#line 83 "minicompg.y"
{yyval=yyvsp[-1];;
    break;}
case 29:
#line 84 "minicompg.y"
{yyval=number_node();;
    break;}
case 30:
#line 85 "minicompg.y"
{yyval=binary_node(NCALL,yyvsp[-3],yyvsp[-1]);;
    break;}
case 31:
#line 86 "minicompg.y"
{yyval=binary_node(NCALL,yyvsp[-2],0);;
    break;}
case 33:
#line 91 "minicompg.y"
{yyval=binary_node(NINDEX,yyvsp[-3],yyvsp[-1]);;
    break;}
case 35:
#line 96 "minicompg.y"
{yyval=var_node();;
    break;}
case 36:
#line 100 "minicompg.y"
{yyval=binary_node(NARG,yyvsp[-2],yyvsp[0]);;
    break;}
case 45:
#line 118 "minicompg.y"
{enter_block();;
    break;}
case 47:
#line 122 "minicompg.y"
{leave_block();;
    break;}
case 48:
#line 123 "minicompg.y"
{leave_block();;
    break;}
case 49:
#line 124 "minicompg.y"
{leave_block();;
    break;}
case 50:
#line 125 "minicompg.y"
{leave_block();;
    break;}
case 51:
#line 130 "minicompg.y"
{if_statement(yyvsp[-1]);;
    break;}
case 53:
#line 134 "minicompg.y"
{if_end();;
    break;}
case 54:
#line 135 "minicompg.y"
{if_else();;
    break;}
case 55:
#line 135 "minicompg.y"
{if_end();;
    break;}
case 56:
#line 140 "minicompg.y"
{while_statement(yyvsp[-1]);;
    break;}
case 57:
#line 140 "minicompg.y"
{while_end();;
    break;}
case 58:
#line 144 "minicompg.y"
{assign_statement(yyvsp[-3],yyvsp[-1]);;
    break;}
case 59:
#line 148 "minicompg.y"
{return_statement(yyvsp[-1]);;
    break;}
case 61:
#line 156 "minicompg.y"
{enter_func(yyvsp[-1],yyvsp[-2]);;
    break;}
case 62:
#line 160 "minicompg.y"
{leave_func();;
    break;}
case 63:
#line 161 "minicompg.y"
{leave_func();;
    break;}
case 66:
#line 170 "minicompg.y"
{add_var(yyvsp[0],yyvsp[-1]);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 542 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 173 "minicompg.y"


#include "minicomplexer.c"

