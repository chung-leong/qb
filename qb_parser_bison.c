
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "qb_parser_bison.y"

/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chung Leong <cleong@cal.berkeley.edu>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "qb.h"
#include "qb_parser_re2c.h"



/* Line 189 of yacc.c  */
#line 100 "qb_parser_bison.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 32 "qb_parser_bison.y"

#define YYLTYPE qb_token_position
#define YYLLOC_DEFAULT(Cur, Rhs, N)            \
do                                             \
  if (N)                                       \
    {                                          \
      (Cur).index   = YYRHSLOC(Rhs, 1).index;  \
      (Cur).length  = YYRHSLOC(Rhs, N).index   \
                    + YYRHSLOC(Rhs, N).length  \
                    - YYRHSLOC(Rhs, 1).index;  \
    }                                          \
  else                                         \
    {                                          \
      (Cur).index  += (Cur).length;            \
      (Cur).length  = 0;                       \
    }                                          \
while (0)                                      \




/* Line 209 of yacc.c  */
#line 146 "qb_parser_bison.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_FUNCTION_SELECTOR = 258,
     T_CLASS_SELECTOR = 259,
     T_PROPERTY_SELECTOR = 260,
     T_TYPE_INT = 261,
     T_TYPE_INT08 = 262,
     T_TYPE_INT16 = 263,
     T_TYPE_INT32 = 264,
     T_TYPE_INT64 = 265,
     T_TYPE_UINT = 266,
     T_TYPE_UINT08 = 267,
     T_TYPE_UINT16 = 268,
     T_TYPE_UINT32 = 269,
     T_TYPE_UINT64 = 270,
     T_TYPE_FLOAT = 271,
     T_TYPE_DOUBLE = 272,
     T_TYPE_FLOAT32 = 273,
     T_TYPE_FLOAT64 = 274,
     T_TYPE_VOID = 275,
     T_TYPE_BOOLEAN = 276,
     T_TYPE_STRING = 277,
     T_TYPE_CHAR = 278,
     T_TYPE_IMAGE = 279,
     T_TYPE_IMAGE4 = 280,
     T_TYPE_COMPLEX = 281,
     T_TYPE_VECTOR = 282,
     T_TYPE_VECTOR3 = 283,
     T_TYPE_MATRIX = 284,
     T_TYPE_MATRIX3X3 = 285,
     T_TAG_ENGINE = 286,
     T_TAG_INLINE = 287,
     T_TAG_IMPORT = 288,
     T_TAG_GLOBAL = 289,
     T_TAG_LOCAL = 290,
     T_TAG_SHARED = 291,
     T_TAG_LEXICAL = 292,
     T_TAG_PARAM = 293,
     T_TAG_STATIC = 294,
     T_TAG_PROPERTY = 295,
     T_TAG_VAR = 296,
     T_TAG_RETURN = 297,
     T_TAG_RECEIVE = 298,
     T_NEVER = 299,
     T_ALWAYS = 300,
     T_ENGINE_QB = 301,
     T_ENGINE_QB_NATIVE = 302,
     T_ENGINE_QB_BYTECODE = 303,
     T_LABEL = 304,
     T_REGEXP = 305,
     T_DECIMAL = 306,
     T_HEXADECIMAL = 307,
     T_COMMENT = 308,
     T_UNEXPECTED = 309
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 72 "qb_parser_bison.y"

	uint32_t count;
	uint32_t flags;
	qb_primitive_type primitive_type;
	qb_matrix_dimension matrix;
	qb_token_position token;
	qb_index_alias_scheme *alias_scheme;



/* Line 214 of yacc.c  */
#line 228 "qb_parser_bison.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* "%code provides" blocks.  */

/* Line 261 of yacc.c  */
#line 52 "qb_parser_bison.y"

#define yylex		qb_doc_comment_yylex
#define yyparse		qb_doc_comment_yyparse
#define yyerror		qb_doc_comment_yyerror

int qb_doc_comment_yylex(YYSTYPE *lvalp, YYLTYPE *locp, qb_parser_context *cxt);
int qb_doc_comment_yyerror(YYLTYPE *locp, qb_parser_context *cxt, const char *msg);

#define qb_clean_read_ahead_token(cxt)	\
	yychar = YYEMPTY;\

#define qb_end_statement(cxt) \
	if(yychar != YYEMPTY) {\
		cxt->lexer_context->cursor = cxt->lexer_context->base + yylval.token.index;\
		yychar = YYEMPTY;\
	}\
	cxt->lexer_context->condition = yycCOMMENT;\




/* Line 261 of yacc.c  */
#line 274 "qb_parser_bison.c"

/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 280 "qb_parser_bison.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   135

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNRULES -- Number of states.  */
#define YYNSTATES  139

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    61,     2,     2,     2,
       2,     2,    59,     2,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    62,     2,
      57,     2,    58,    60,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    55,     2,    56,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    13,    16,    17,    20,
      21,    24,    27,    30,    33,    36,    38,    41,    43,    46,
      48,    51,    53,    55,    57,    60,    62,    64,    67,    71,
      74,    77,    79,    81,    83,    85,    87,    89,    91,    93,
      97,    99,   101,   104,   106,   109,   112,   115,   117,   120,
     123,   126,   129,   132,   134,   136,   138,   140,   143,   145,
     148,   150,   152,   155,   157,   160,   162,   165,   167,   170,
     172,   175,   177,   181,   185,   187,   189,   191,   193,   195,
     197,   199,   201,   203,   205,   207,   209,   211,   213,   214,
     217,   221,   224,   228,   232,   236,   240,   242,   244,   246,
     250,   254
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      65,     0,    -1,     3,    66,    -1,     4,    67,    -1,     5,
      68,    -1,    -1,    66,    69,    -1,    -1,    67,    70,    -1,
      -1,    68,    71,    -1,    77,    53,    -1,    72,    53,    -1,
      74,    53,    -1,    76,    53,    -1,    53,    -1,    81,    53,
      -1,    53,    -1,    83,    53,    -1,    53,    -1,    31,    73,
      -1,    46,    -1,    47,    -1,    48,    -1,    32,    75,    -1,
      44,    -1,    45,    -1,    33,    49,    -1,    78,    86,    85,
      -1,    79,    87,    -1,    80,    86,    -1,    34,    -1,    35,
      -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,    42,
      -1,    43,    -1,    82,    86,    85,    -1,    39,    -1,    40,
      -1,    84,    86,    -1,    41,    -1,    61,    49,    -1,    61,
      50,    -1,    88,    99,    -1,    89,    -1,    90,    99,    -1,
      91,    99,    -1,    92,    99,    -1,    93,    99,    -1,    94,
      99,    -1,    86,    -1,    20,    -1,    97,    -1,    98,    -1,
      22,    95,    -1,    22,    -1,    23,    95,    -1,    23,    -1,
      21,    -1,    24,    96,    -1,    24,    -1,    25,    96,    -1,
      25,    -1,    27,    96,    -1,    27,    -1,    28,    96,    -1,
      28,    -1,    26,    96,    -1,    26,    -1,    57,    97,    58,
      -1,    57,    98,    58,    -1,     7,    -1,     8,    -1,     9,
      -1,    10,    -1,     6,    -1,    12,    -1,    13,    -1,    14,
      -1,    15,    -1,    11,    -1,    18,    -1,    19,    -1,    16,
      -1,    17,    -1,    -1,    99,   100,    -1,    55,   101,    56,
      -1,    55,    56,    -1,    55,    60,    56,    -1,    55,    59,
      56,    -1,    55,    49,    56,    -1,    55,   102,    56,    -1,
      51,    -1,    52,    -1,   103,    -1,    49,    62,   103,    -1,
      49,    63,    49,    -1,   103,    63,    49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   155,   155,   156,   157,   160,   162,   165,   167,   170,
     172,   176,   177,   178,   179,   180,   184,   185,   189,   190,
     194,   198,   199,   200,   204,   208,   209,   213,   217,   218,
     219,   223,   224,   225,   226,   227,   228,   232,   236,   240,
     244,   245,   249,   253,   257,   258,   262,   263,   264,   265,
     266,   267,   268,   272,   273,   276,   277,   281,   282,   286,
     287,   291,   295,   296,   297,   298,   302,   303,   304,   305,
     309,   310,   314,   317,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   333,   334,   335,   336,   339,   341,
     345,   346,   347,   348,   349,   350,   354,   355,   359,   360,
     363,   364
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_FUNCTION_SELECTOR",
  "T_CLASS_SELECTOR", "T_PROPERTY_SELECTOR", "T_TYPE_INT", "T_TYPE_INT08",
  "T_TYPE_INT16", "T_TYPE_INT32", "T_TYPE_INT64", "T_TYPE_UINT",
  "T_TYPE_UINT08", "T_TYPE_UINT16", "T_TYPE_UINT32", "T_TYPE_UINT64",
  "T_TYPE_FLOAT", "T_TYPE_DOUBLE", "T_TYPE_FLOAT32", "T_TYPE_FLOAT64",
  "T_TYPE_VOID", "T_TYPE_BOOLEAN", "T_TYPE_STRING", "T_TYPE_CHAR",
  "T_TYPE_IMAGE", "T_TYPE_IMAGE4", "T_TYPE_COMPLEX", "T_TYPE_VECTOR",
  "T_TYPE_VECTOR3", "T_TYPE_MATRIX", "T_TYPE_MATRIX3X3", "T_TAG_ENGINE",
  "T_TAG_INLINE", "T_TAG_IMPORT", "T_TAG_GLOBAL", "T_TAG_LOCAL",
  "T_TAG_SHARED", "T_TAG_LEXICAL", "T_TAG_PARAM", "T_TAG_STATIC",
  "T_TAG_PROPERTY", "T_TAG_VAR", "T_TAG_RETURN", "T_TAG_RECEIVE",
  "T_NEVER", "T_ALWAYS", "T_ENGINE_QB", "T_ENGINE_QB_NATIVE",
  "T_ENGINE_QB_BYTECODE", "T_LABEL", "T_REGEXP", "T_DECIMAL",
  "T_HEXADECIMAL", "T_COMMENT", "T_UNEXPECTED", "'['", "']'", "'<'", "'>'",
  "'*'", "'?'", "'$'", "':'", "','", "$accept", "input", "func_input",
  "class_input", "prop_input", "func_line", "class_line", "prop_line",
  "engine_attr", "engine_type", "inline_attr", "inline_type",
  "import_stat", "func_type_decl", "func_var_scope", "return", "receive",
  "class_type_decl", "class_var_scope", "prop_type_decl", "prop_var_scope",
  "var_name", "var_type", "var_type_or_void", "primitive_type",
  "string_type", "char_type", "boolean_type", "image_type", "vector_type",
  "complex_type", "int_type_base", "float_type_base", "int_type",
  "float_type", "array_dimensions", "array_dimension", "element_count",
  "alias_scheme", "name_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,    91,    93,    60,    62,    42,
      63,    36,    58,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    64,    65,    65,    65,    66,    66,    67,    67,    68,
      68,    69,    69,    69,    69,    69,    70,    70,    71,    71,
      72,    73,    73,    73,    74,    75,    75,    76,    77,    77,
      77,    78,    78,    78,    78,    78,    78,    79,    80,    81,
      82,    82,    83,    84,    85,    85,    86,    86,    86,    86,
      86,    86,    86,    87,    87,    88,    88,    89,    89,    90,
      90,    91,    92,    92,    92,    92,    93,    93,    93,    93,
      94,    94,    95,    96,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    98,    98,    98,    98,    99,    99,
     100,   100,   100,   100,   100,   100,   101,   101,   102,   102,
     103,   103
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     2,     0,     2,     0,     2,     0,
       2,     2,     2,     2,     2,     1,     2,     1,     2,     1,
       2,     1,     1,     1,     2,     1,     1,     2,     3,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     2,     1,     2,     2,     2,     1,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     2,
       3,     2,     3,     3,     3,     3,     1,     1,     1,     3,
       3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,     7,     9,     0,     2,     3,     4,     1,     0,
       0,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      15,     6,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    17,     8,     0,     0,    43,    19,    10,     0,     0,
      21,    22,    23,    20,    25,    26,    24,    27,    12,    13,
      14,    11,    78,    74,    75,    76,    77,    83,    79,    80,
      81,    82,    86,    87,    84,    85,    61,    58,    60,    63,
      65,    71,    67,    69,     0,    88,    47,    88,    88,    88,
      88,    88,    55,    56,    54,    53,    29,    30,    16,     0,
      18,    42,     0,    57,    59,     0,    62,    64,    70,    66,
      68,     0,    28,    46,    48,    49,    50,    51,    52,    39,
       0,     0,    44,    45,     0,    89,    72,    73,     0,    96,
      97,    91,     0,     0,     0,     0,    98,    94,     0,     0,
      93,    92,    90,    95,     0,     0,    99,   100,   101
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    21,    32,    37,    22,    43,
      23,    46,    24,    25,    26,    27,    28,    33,    34,    38,
      39,   102,    74,    86,    75,    76,    77,    78,    79,    80,
      81,    93,    96,    82,    83,   103,   115,   124,   125,   126
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -7
static const yytype_int8 yypact[] =
{
     101,    -7,    -7,    -7,    37,    15,    16,    20,    -7,    61,
      36,    29,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    41,    57,    60,    63,    17,    -6,    17,    -7,
      -7,    -7,    -7,    64,    17,    -7,    -7,    -7,    65,    17,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    10,    10,    58,
      58,    58,    58,    58,    53,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    53,
      -7,    -7,    78,    -7,    -7,    46,    -7,    -7,    -7,    -7,
      -7,    62,    -7,    66,    66,    66,    66,    66,    66,    -7,
      67,    68,    -7,    -7,    23,    -7,    -7,    -7,    14,    -7,
      -7,    -7,    71,    72,    73,    74,    56,    -7,    75,    82,
      -7,    -7,    -7,    -7,    83,    59,    56,    -7,    -7
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    31,    32,    -7,    -7,    -7,    -7,    -7,    -7,    -7,
      -7,    55,    30,    42,    38,    18,    -7,    -7,    -7,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    84,    66,    67,    68,    69,    70,
      71,    72,    73,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,     8,    66,    67,
      68,    69,    70,    71,    72,    73,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    29,    30,    18,    19,    85,
      87,    35,    62,    63,    64,    65,    89,    92,    20,    31,
     127,    91,   118,    36,   119,   120,   128,   129,    47,   121,
      44,    45,   122,   123,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    48,   104,   105,   106,   107,   108,
      97,    98,    99,   100,     1,     2,     3,    40,    41,    42,
      49,   112,   113,    50,   101,    95,    51,    88,    90,   134,
     109,   114,   129,    94,   135,   116,   117,   130,   131,   132,
     133,   137,   138,   111,   110,   136
};

static const yytype_uint8 yycheck[] =
{
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    39,    40,    42,    43,    27,
      28,    41,    16,    17,    18,    19,    34,    57,    53,    53,
      56,    39,    49,    53,    51,    52,    62,    63,    49,    56,
      44,    45,    59,    60,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    53,    77,    78,    79,    80,    81,
      70,    71,    72,    73,     3,     4,     5,    46,    47,    48,
      53,    49,    50,    53,    61,    57,    53,    53,    53,    63,
      89,    55,    63,    68,    49,    58,    58,    56,    56,    56,
      56,    49,    49,    95,    92,   128
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,    65,    66,    67,    68,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    42,    43,
      53,    69,    72,    74,    76,    77,    78,    79,    80,    39,
      40,    53,    70,    81,    82,    41,    53,    71,    83,    84,
      46,    47,    48,    73,    44,    45,    75,    49,    53,    53,
      53,    53,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    21,    22,    23,    24,
      25,    26,    27,    28,    86,    88,    89,    90,    91,    92,
      93,    94,    97,    98,    20,    86,    87,    86,    53,    86,
      53,    86,    57,    95,    95,    57,    96,    96,    96,    96,
      96,    61,    85,    99,    99,    99,    99,    99,    99,    85,
      97,    98,    49,    50,    55,   100,    58,    58,    49,    51,
      52,    56,    59,    60,   101,   102,   103,    56,    62,    63,
      56,    56,    56,    56,    63,    49,   103,    49,    49
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, cxt, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, cxt)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, cxt); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, qb_parser_context *cxt)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, cxt)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    qb_parser_context *cxt;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (cxt);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, qb_parser_context *cxt)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, cxt)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    qb_parser_context *cxt;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, cxt);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, qb_parser_context *cxt)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, cxt)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    qb_parser_context *cxt;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , cxt);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, cxt); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, qb_parser_context *cxt)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, cxt)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    qb_parser_context *cxt;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (cxt);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (qb_parser_context *cxt);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (qb_parser_context *cxt)
#else
int
yyparse (cxt)
    qb_parser_context *cxt;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 20:

/* Line 1455 of yacc.c  */
#line 194 "qb_parser_bison.y"
    { if(!qb_set_engine_flags(cxt, (yyvsp[(2) - (2)].flags), (yylsp[(2) - (2)]))) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 198 "qb_parser_bison.y"
    { (yyval.flags) = 0; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 199 "qb_parser_bison.y"
    { (yyval.flags) = QB_FUNCTION_NATIVE_IF_POSSIBLE; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 200 "qb_parser_bison.y"
    { (yyval.flags) = QB_FUNCTION_NEVER_NATIVE; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 204 "qb_parser_bison.y"
    { if(!qb_set_engine_flags(cxt, (yyvsp[(2) - (2)].flags), (yylsp[(1) - (2)]))) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 208 "qb_parser_bison.y"
    { (yyval.flags) = QB_FUNCTION_NEVER_INLINE; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 209 "qb_parser_bison.y"
    { (yyval.flags) = QB_FUNCTION_INLINE_ALWAYS; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 213 "qb_parser_bison.y"
    { if(!qb_add_import(cxt, (yyvsp[(2) - (2)].token))) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 217 "qb_parser_bison.y"
    { if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 218 "qb_parser_bison.y"
    { if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 219 "qb_parser_bison.y"
    { if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 223 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_GLOBAL, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 224 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_LOCAL, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 225 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_SHARED, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 226 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_LEXICAL, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 227 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_ARGUMENT, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 228 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_STATIC, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 232 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_RETURN_VALUE, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 236 "qb_parser_bison.y"
    { if(!qb_add_variable_declaration(cxt, QB_VARIABLE_SENT_VALUE, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 240 "qb_parser_bison.y"
    { if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 244 "qb_parser_bison.y"
    { if(!qb_add_property_declaration(cxt, QB_VARIABLE_CLASS, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 245 "qb_parser_bison.y"
    { if(!qb_add_property_declaration(cxt, QB_VARIABLE_CLASS_INSTANCE, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 249 "qb_parser_bison.y"
    { if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 253 "qb_parser_bison.y"
    { if(!qb_add_property_declaration(cxt, 0, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 257 "qb_parser_bison.y"
    { if(!qb_attach_variable_name(cxt, (yyvsp[(2) - (2)].token))) YYABORT; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 258 "qb_parser_bison.y"
    { if(!qb_attach_variable_name_regexp(cxt, (yyvsp[(2) - (2)].token))) YYABORT; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 263 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 0, 0, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 266 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 0, 0, (yylsp[(1) - (2)]))) YYABORT; qb_add_dimension(cxt, 0, 0, (yylsp[(1) - (2)])); qb_add_dimension(cxt, (yyvsp[(1) - (2)].count), 0, (yylsp[(1) - (2)])); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 267 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, (yyvsp[(1) - (2)].count), 0, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 268 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 2, 0, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 273 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, QB_TYPE_VOID, 0, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 276 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, (yyvsp[(1) - (1)].primitive_type), 0, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 277 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, (yyvsp[(1) - (1)].primitive_type), 0, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 281 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_STRING, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 282 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 286 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_STRING, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 287 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 291 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, QB_TYPE_I32, QB_TYPE_DECL_BOOLEAN, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 295 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (2)].token), 10); if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_IMAGE, (yylsp[(1) - (2)]))) YYABORT;;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 296 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (1)].token), 10); if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 297 "qb_parser_bison.y"
    { (yyval.count) = 4; if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_IMAGE, (yylsp[(1) - (2)]))) YYABORT;;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 298 "qb_parser_bison.y"
    { (yyval.count) = 4; if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 302 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (2)].token), 10); if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_VECTOR, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 303 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (1)].token), 10); if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 304 "qb_parser_bison.y"
    { (yyval.count) = 3; if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_VECTOR, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 305 "qb_parser_bison.y"
    { (yyval.count) = 3; if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 309 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, (yyvsp[(2) - (2)].primitive_type), QB_TYPE_DECL_COMPLEX, (yylsp[(1) - (2)]))) YYABORT; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 310 "qb_parser_bison.y"
    { if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_COMPLEX, (yylsp[(1) - (1)]))) YYABORT; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 314 "qb_parser_bison.y"
    { (yyval.primitive_type) = (yyvsp[(2) - (3)].primitive_type); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 317 "qb_parser_bison.y"
    { (yyval.primitive_type) = (yyvsp[(2) - (3)].primitive_type); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 320 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_I08; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 321 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_I16; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 322 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_I32; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 323 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_I64; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 324 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_I32; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 325 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_U08; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 326 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_U16; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 327 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_U32; ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 328 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_U64; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 329 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_U32; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 333 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_F32; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 334 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_F64; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 335 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_F32; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 336 "qb_parser_bison.y"
    { (yyval.primitive_type) = QB_TYPE_F64; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 345 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, (yyvsp[(2) - (3)].count), 0, (yylsp[(2) - (3)]))) YYABORT; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 346 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 0, 0, (yylsp[(2) - (2)]))) YYABORT; ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 347 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 0, 0, (yylsp[(2) - (3)]))) YYABORT; ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 348 "qb_parser_bison.y"
    { if(!qb_add_dimension(cxt, 0, QB_TYPE_DECL_AUTOVIVIFICIOUS, (yylsp[(2) - (3)]))) YYABORT; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 349 "qb_parser_bison.y"
    { if(!qb_parse_constant(cxt, (yyvsp[(2) - (3)].token))) YYABORT; qb_clean_read_ahead_token(cxt); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 350 "qb_parser_bison.y"
    { if(!qb_add_index_alias_scheme(cxt, (yyvsp[(2) - (3)].alias_scheme), (yylsp[(2) - (3)]))) YYABORT; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 354 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (1)].token), 10); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 355 "qb_parser_bison.y"
    { (yyval.count) = qb_parse_integer(cxt, (yyvsp[(1) - (1)].token), 16); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 359 "qb_parser_bison.y"
    { (yyval.alias_scheme) = (yyvsp[(1) - (1)].alias_scheme); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 360 "qb_parser_bison.y"
    { (yyval.alias_scheme) = (yyvsp[(3) - (3)].alias_scheme); if(!qb_attach_index_alias_scheme_class(cxt, (yyval.alias_scheme), (yyvsp[(1) - (3)].token))) YYABORT; ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 363 "qb_parser_bison.y"
    { (yyval.alias_scheme) = qb_create_index_alias_scheme(cxt); if(!qb_add_index_alias(cxt, (yyval.alias_scheme), (yyvsp[(1) - (3)].token))) YYABORT; if(!qb_add_index_alias(cxt, (yyval.alias_scheme), (yyvsp[(3) - (3)].token))) YYABORT; ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 364 "qb_parser_bison.y"
    { (yyval.alias_scheme) = (yyvsp[(1) - (3)].alias_scheme); if(!qb_add_index_alias(cxt, (yyval.alias_scheme), (yyvsp[(3) - (3)].token))) YYABORT; ;}
    break;



/* Line 1455 of yacc.c  */
#line 2200 "qb_parser_bison.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, cxt, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, cxt, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, cxt, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, cxt);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, cxt);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, cxt, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, cxt);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, cxt);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 367 "qb_parser_bison.y"


