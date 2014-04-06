
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
#line 31 "qb_parser_bison.y"

	uint32_t count;
	uint32_t flags;
	qb_primitive_type primitive_type;
	qb_matrix_dimension matrix;
	qb_token_position token;
	qb_index_alias_scheme *alias_scheme;



/* Line 1676 of yacc.c  */
#line 117 "qb_parser_bison.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




/* "%code provides" blocks.  */

/* Line 1676 of yacc.c  */
#line 12 "qb_parser_bison.y"

#define yylex		qb_doc_comment_yylex
#define yyparse		qb_doc_comment_yyparse
#define yyerror		qb_doc_comment_yyerror

int qb_doc_comment_yylex(YYSTYPE *lvalp, qb_parser_context *cxt);

#define qb_clean_read_ahead_token(cxt)	\
	yychar = YYEMPTY;\

#define qb_end_statement(cxt) \
	if(yychar != YYEMPTY) {\
		cxt->lexer_context->cursor = cxt->lexer_context->base + yylval.token.index;\
		yychar = YYEMPTY;\
	}\
	cxt->lexer_context->condition = yycCOMMENT;\




/* Line 1676 of yacc.c  */
#line 152 "qb_parser_bison.h"
