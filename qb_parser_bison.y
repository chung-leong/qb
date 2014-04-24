%{
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

%}
	
%define api.pure
%parse-param	{ qb_parser_context *cxt }
%lex-param		{ qb_parser_context *cxt }
%locations

%code requires {
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

}

%code provides {
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

}

%union {
	uint32_t count;
	uint32_t flags;
	qb_primitive_type primitive_type;
	qb_matrix_dimension matrix;
	qb_token_position token;
	qb_index_alias_scheme *alias_scheme;
}

%token T_FUNCTION_SELECTOR
%token T_CLASS_SELECTOR 
%token T_PROPERTY_SELECTOR
 
%token T_TYPE_INT
%token T_TYPE_INT08
%token T_TYPE_INT16
%token T_TYPE_INT32
%token T_TYPE_INT64

%token T_TYPE_UINT
%token T_TYPE_UINT08
%token T_TYPE_UINT16
%token T_TYPE_UINT32
%token T_TYPE_UINT64

%token T_TYPE_FLOAT
%token T_TYPE_DOUBLE
%token T_TYPE_FLOAT32
%token T_TYPE_FLOAT64

%token T_TYPE_VOID
%token T_TYPE_BOOLEAN
%token T_TYPE_STRING
%token T_TYPE_CHAR
%token T_TYPE_IMAGE
%token T_TYPE_IMAGE4
%token T_TYPE_COMPLEX
%token T_TYPE_VECTOR
%token T_TYPE_VECTOR3
%token T_TYPE_MATRIX
%token T_TYPE_MATRIX3X3

%token T_TAG_ENGINE
%token T_TAG_INLINE
%token T_TAG_IMPORT

%token T_TAG_GLOBAL
%token T_TAG_LOCAL
%token T_TAG_SHARED
%token T_TAG_LEXICAL
%token T_TAG_PARAM
%token T_TAG_STATIC
%token T_TAG_PROPERTY
%token T_TAG_VAR
%token T_TAG_RETURN
%token T_TAG_RECEIVE

%token T_NEVER
%token T_ALWAYS
%token T_ENGINE_QB				
%token T_ENGINE_QB_NATIVE
%token T_ENGINE_QB_BYTECODE

%token T_LABEL
%token T_REGEXP
%token T_DECIMAL
%token T_HEXADECIMAL

%token T_COMMENT
%token T_UNEXPECTED

%token '[' ']' '<' '>' '*' '?' '$' ':' ','

%type <flags>			engine_type inline_type
%type <primitive_type>	int_type float_type int_type_base float_type_base
%type <count>			element_count image_type vector_type
%type <matrix>			matrix_type
%type <alias_scheme>	alias_scheme name_list

%type <token>			T_LABEL T_REGEXP T_DECIMAL T_HEXADECIMAL T_TYPE_IMAGE T_TYPE_VECTOR T_TYPE_MATRIX

%%

input
	: T_FUNCTION_SELECTOR func_input
	| T_CLASS_SELECTOR class_input
	| T_PROPERTY_SELECTOR prop_input
	;

func_input
	: 
	| func_input func_line
	;

class_input
	: 
	| class_input class_line
	;

prop_input
	:
	| prop_input prop_line
	;

func_line
	: func_type_decl T_COMMENT
	| engine_attr T_COMMENT
	| inline_attr T_COMMENT
	| import_stat T_COMMENT
	| T_COMMENT
	;
	
class_line
	: class_type_decl T_COMMENT
	| T_COMMENT
	;
	
prop_line
	: prop_type_decl T_COMMENT
	| T_COMMENT
	;	
	
engine_attr
	: T_TAG_ENGINE engine_type			{ if(!qb_set_engine_flags(cxt, $2, @2)) YYABORT; qb_end_statement(cxt); }
	;

engine_type
	: T_ENGINE_QB						{ $$ = 0; }
	| T_ENGINE_QB_NATIVE				{ $$ = QB_FUNCTION_NATIVE_IF_POSSIBLE; }	
	| T_ENGINE_QB_BYTECODE				{ $$ = QB_FUNCTION_NEVER_NATIVE; }	
	;
	
inline_attr
	: T_TAG_INLINE inline_type			{ if(!qb_set_engine_flags(cxt, $2, @1)) YYABORT; qb_end_statement(cxt); }
	;
	
inline_type
	: T_NEVER							{ $$ = QB_FUNCTION_NEVER_INLINE; }
	| T_ALWAYS							{ $$ = QB_FUNCTION_INLINE_ALWAYS; }
	;

import_stat
	: T_TAG_IMPORT T_LABEL				{ if(!qb_add_import(cxt, $2)) YYABORT; qb_end_statement(cxt); }
	;

func_type_decl
	: func_var_scope var_type var_name	{ if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); }
	| return var_type_or_void 			{ if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); }
	| receive var_type					{ if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); }
	;

func_var_scope
	: T_TAG_GLOBAL						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_GLOBAL, @1)) YYABORT; }
	| T_TAG_LOCAL						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_LOCAL, @1)) YYABORT; }
	| T_TAG_SHARED						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_SHARED, @1)) YYABORT; }
	| T_TAG_LEXICAL						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_LEXICAL, @1)) YYABORT; }
	| T_TAG_PARAM						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_ARGUMENT, @1)) YYABORT; }
	| T_TAG_STATIC						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_STATIC, @1)) YYABORT; }
	;

return
	: T_TAG_RETURN						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_RETURN_VALUE, @1)) YYABORT; }
	;
	 
receive
	: T_TAG_RECEIVE						{ if(!qb_add_variable_declaration(cxt, QB_VARIABLE_SENT_VALUE, @1)) YYABORT; }
	;
	
class_type_decl
	: class_var_scope var_type var_name	{ if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); }
	;
	
class_var_scope
	: T_TAG_STATIC						{ if(!qb_add_property_declaration(cxt, QB_VARIABLE_CLASS, @1)) YYABORT; }
	| T_TAG_PROPERTY					{ if(!qb_add_property_declaration(cxt, QB_VARIABLE_CLASS_INSTANCE, @1)) YYABORT; }
	;
	
prop_type_decl
	: prop_var_scope var_type			{ if(!qb_end_variable_declaration(cxt)) YYABORT; qb_end_statement(cxt); }
	;
		
prop_var_scope
	: T_TAG_VAR							{ if(!qb_add_property_declaration(cxt, 0, @1)) YYABORT; }
	;
	
var_name
	: '$' T_LABEL						{ if(!qb_attach_variable_name(cxt, $2)) YYABORT; }
	| '$' T_REGEXP						{ if(!qb_attach_variable_name_regexp(cxt, $2)) YYABORT; }
	;

var_type
	: primitive_type array_dimensions
	| string_type						{ if(!qb_add_dimension(cxt, 0, 0, @1)) YYABORT; }
	| char_type array_dimensions
	| boolean_type array_dimensions
	| image_type array_dimensions		{ if(!qb_add_dimension(cxt, 0, 0, @1)) YYABORT; qb_add_dimension(cxt, 0, 0, @1); qb_add_dimension(cxt, $1, 0, @1); }
	| vector_type array_dimensions		{ if(!qb_add_dimension(cxt, $1, 0, @1)) YYABORT; }
	| matrix_type array_dimensions		{ if(!qb_add_matrix_dimension(cxt, $1, @1)) YYABORT; }
	| complex_type array_dimensions		{ if(!qb_add_dimension(cxt, 2, 0, @1)) YYABORT; }
	;
	
var_type_or_void
	: var_type
	| T_TYPE_VOID						{ if(!qb_set_variable_type(cxt, QB_TYPE_VOID, 0, @1)) YYABORT; }

primitive_type							
	: int_type							{ if(!qb_set_variable_type(cxt, $1, 0, @1)) YYABORT; }
	| float_type						{ if(!qb_set_variable_type(cxt, $1, 0, @1)) YYABORT; }
	;

string_type
	: T_TYPE_STRING int_type_base		{ if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_STRING, @1)) YYABORT; }
	| T_TYPE_STRING						{ if(!qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING, @1)) YYABORT; }
	;

char_type
	: T_TYPE_CHAR int_type_base			{ if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_STRING, @1)) YYABORT; }
	| T_TYPE_CHAR						{ if(!qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING, @1)) YYABORT; }
	;

boolean_type
	: T_TYPE_BOOLEAN					{ if(!qb_set_variable_type(cxt, QB_TYPE_I32, QB_TYPE_DECL_BOOLEAN, @1)) YYABORT; }
	;
	
image_type
	: T_TYPE_IMAGE float_type_base		{ $$ = qb_parse_integer(cxt, $1, 10); if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_IMAGE, @1)) YYABORT;}
	| T_TYPE_IMAGE						{ $$ = qb_parse_integer(cxt, $1, 10); if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE, @1)) YYABORT; }
	| T_TYPE_IMAGE4 float_type_base		{ $$ = 4; if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_IMAGE, @1)) YYABORT;}
	| T_TYPE_IMAGE4						{ $$ = 4; if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE, @1)) YYABORT; }
	;

vector_type
	: T_TYPE_VECTOR float_type_base		{ $$ = qb_parse_integer(cxt, $1, 10); if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_VECTOR, @1)) YYABORT; }
	| T_TYPE_VECTOR						{ $$ = qb_parse_integer(cxt, $1, 10); if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR, @1)) YYABORT; }
	| T_TYPE_VECTOR3 float_type_base	{ $$ = 3; if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_VECTOR, @1)) YYABORT; }
	| T_TYPE_VECTOR3					{ $$ = 3; if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR, @1)) YYABORT; }
	;

matrix_type
	: T_TYPE_MATRIX float_type_base		{ $$ = qb_parse_dimension(cxt, $1); if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_MATRIX, @1)) YYABORT; }
	| T_TYPE_MATRIX						{ $$ = qb_parse_dimension(cxt, $1); if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_MATRIX, @1)) YYABORT; }
	| T_TYPE_MATRIX3X3 float_type_base	{ $$.row = 3; $$.column = 3; if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_MATRIX, @1)) YYABORT; }
	| T_TYPE_MATRIX3X3					{ $$.row = 3; $$.column = 3; if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_MATRIX, @1)) YYABORT; }
	;

complex_type
	: T_TYPE_COMPLEX float_type_base	{ if(!qb_set_variable_type(cxt, $2, QB_TYPE_DECL_COMPLEX, @1)) YYABORT; }
	| T_TYPE_COMPLEX					{ if(!qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_COMPLEX, @1)) YYABORT; }
	;

int_type_base
	: '<' int_type '>'					{ $$ = $2; }

float_type_base
	: '<' float_type '>'				{ $$ = $2; }

int_type
	: T_TYPE_INT08						{ $$ = QB_TYPE_I08; }
	| T_TYPE_INT16						{ $$ = QB_TYPE_I16; }
	| T_TYPE_INT32						{ $$ = QB_TYPE_I32; }
	| T_TYPE_INT64						{ $$ = QB_TYPE_I64; }
	| T_TYPE_INT						{ $$ = QB_TYPE_I32; }
	| T_TYPE_UINT08						{ $$ = QB_TYPE_U08; }
	| T_TYPE_UINT16						{ $$ = QB_TYPE_U16; }
	| T_TYPE_UINT32						{ $$ = QB_TYPE_U32; }
	| T_TYPE_UINT64						{ $$ = QB_TYPE_U64; }
	| T_TYPE_UINT						{ $$ = QB_TYPE_U32; }
	;

float_type
	: T_TYPE_FLOAT32					{ $$ = QB_TYPE_F32; }
	| T_TYPE_FLOAT64					{ $$ = QB_TYPE_F64; }
	| T_TYPE_FLOAT						{ $$ = QB_TYPE_F32; }
	| T_TYPE_DOUBLE						{ $$ = QB_TYPE_F64; }
	;

array_dimensions
	: 
	| array_dimensions array_dimension
	;

array_dimension
	: '[' element_count ']'				{ if(!qb_add_dimension(cxt, $2, 0, @2)) YYABORT; }
	| '[' ']'							{ if(!qb_add_dimension(cxt, 0, 0, @2)) YYABORT; }
	| '[' '?' ']'						{ if(!qb_add_dimension(cxt, 0, 0, @2)) YYABORT; }
	| '[' '*' ']'						{ if(!qb_add_dimension(cxt, 0, QB_TYPE_DECL_AUTOVIVIFICIOUS, @2)) YYABORT; }
	| '[' T_LABEL ']'					{ if(!qb_parse_constant(cxt, $2)) YYABORT; qb_clean_read_ahead_token(cxt); }
	| '[' alias_scheme ']'				{ if(!qb_add_index_alias_scheme(cxt, $2, @2)) YYABORT; }
	;
	
element_count
	: T_DECIMAL							{ $$ = qb_parse_integer(cxt, $1, 10); }
	| T_HEXADECIMAL						{ $$ = qb_parse_integer(cxt, $1, 16); }
	;

alias_scheme
	: name_list							{ $$ = $1; }
	| T_LABEL ':' name_list				{ $$ = $3; if(!qb_attach_index_alias_scheme_class(cxt, $$, $1)) YYABORT; }
	
name_list
	: T_LABEL ',' T_LABEL				{ $$ = qb_create_index_alias_scheme(cxt); if(!qb_add_index_alias(cxt, $$, $1)) YYABORT; if(!qb_add_index_alias(cxt, $$, $3)) YYABORT; }
	| name_list ',' T_LABEL				{ $$ = $1; if(!qb_add_index_alias(cxt, $$, $3)) YYABORT; }	
	;

%%
