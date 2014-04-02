%{

#include "qb.h"
#include "qb_parser_re2c.h"

%}
	
%define api.pure
%parse-param	{ qb_parser_context *cxt }
%lex-param		{ qb_parser_context *cxt }

%code provides {
#define yylex		qb_doc_comment_yylex
#define yyparse		qb_doc_comment_yyparse
#define yyerror		qb_doc_comment_yyerror

int qb_doc_comment_yylex(YYSTYPE *lvalp, qb_parser_context *cxt);

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
%type <alias_scheme>	alias_scheme name_list

%type <token>			T_LABEL T_REGEXP T_DECIMAL T_HEXADECIMAL T_TYPE_IMAGE T_TYPE_VECTOR T_TYPE_MATRIX T_COMMENT
%type <token>			T_TAG_ENGINE T_TAG_INLINE T_TAG_IMPORT T_TAG_GLOBAL T_TAG_LOCAL T_TAG_SHARED T_TAG_LEXICAL T_TAG_PARAM T_TAG_STATIC T_TAG_PROPERTY T_TAG_VAR T_TAG_RETURN T_TAG_RECEIVE

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
	: T_TAG_ENGINE engine_type			{ qb_set_engine_flags(cxt, $2); qb_end_statement(cxt); }
	;

engine_type
	: T_ENGINE_QB						{ $$ = 0; }
	| T_ENGINE_QB_NATIVE				{ $$ = QB_FUNCTION_NATIVE_IF_POSSIBLE; }	
	| T_ENGINE_QB_BYTECODE				{ $$ = QB_FUNCTION_NEVER_NATIVE; }	
	;
	
inline_attr
	: T_TAG_INLINE inline_type			{ qb_set_engine_flags(cxt, $2); qb_end_statement(cxt); }
	;
	
inline_type
	: T_NEVER							{ $$ = QB_FUNCTION_NEVER_INLINE; }
	| T_ALWAYS							{ $$ = QB_FUNCTION_INLINE_ALWAYS; }
	;

import_stat
	: T_TAG_IMPORT T_LABEL				{ qb_add_import(cxt, $2); qb_end_statement(cxt); }
	;

func_type_decl
	: func_var_scope var_type var_name	{ qb_end_variable_declaration(cxt); qb_end_statement(cxt); }
	| return var_type_or_void 			{ qb_end_variable_declaration(cxt); qb_end_statement(cxt); }
	| receive var_type					{ qb_end_variable_declaration(cxt); qb_end_statement(cxt); }
	;

func_var_scope
	: T_TAG_GLOBAL						{ qb_add_variable_declaration(cxt, QB_VARIABLE_GLOBAL); }
	| T_TAG_LOCAL						{ qb_add_variable_declaration(cxt, QB_VARIABLE_LOCAL); }
	| T_TAG_SHARED						{ qb_add_variable_declaration(cxt, QB_VARIABLE_SHARED); }
	| T_TAG_LEXICAL						{ qb_add_variable_declaration(cxt, QB_VARIABLE_LEXICAL); }
	| T_TAG_PARAM						{ qb_add_variable_declaration(cxt, QB_VARIABLE_ARGUMENT); }
	| T_TAG_STATIC						{ qb_add_variable_declaration(cxt, QB_VARIABLE_STATIC); }
	;

return
	: T_TAG_RETURN						{ qb_add_variable_declaration(cxt, QB_VARIABLE_RETURN_VALUE); }
	;
	 
receive
	: T_TAG_RECEIVE						{ qb_add_variable_declaration(cxt, QB_VARIABLE_SENT_VALUE); }
	;
	
class_type_decl
	: class_var_scope var_type var_name	{ qb_end_variable_declaration(cxt); qb_end_statement(cxt); }
	;
	
class_var_scope
	: T_TAG_STATIC						{ qb_add_property_declaration(cxt, QB_VARIABLE_CLASS); }
	| T_TAG_PROPERTY					{ qb_add_property_declaration(cxt, QB_VARIABLE_CLASS_INSTANCE); }
	;
	
prop_type_decl
	: prop_var_scope var_type			{ qb_end_variable_declaration(cxt); qb_end_statement(cxt); }
	;
		
prop_var_scope
	: T_TAG_VAR							{ qb_add_property_declaration(cxt, 0); }
	;
	
var_name
	: '$' T_LABEL						{ qb_attach_variable_name(cxt, $2); }
	| '$' T_REGEXP						{ qb_attach_variable_name_regexp(cxt, $2); }
	;

var_type
	: primitive_type array_dimensions
	| string_type						{ qb_add_dimension(cxt, 0, 0); }
	| char_type array_dimensions
	| boolean_type array_dimensions
	| image_type array_dimensions		{ qb_add_dimension(cxt, 0, 0); qb_add_dimension(cxt, 0, 0); qb_add_dimension(cxt, $1, 0); }
	| vector_type array_dimensions		{ qb_add_dimension(cxt, $1, 0); }
	| complex_type array_dimensions		{ qb_add_dimension(cxt, 2, 0); }
	;
	
var_type_or_void
	: var_type
	| T_TYPE_VOID						{ qb_set_variable_type(cxt, QB_TYPE_VOID, 0); }

primitive_type							
	: int_type							{ qb_set_variable_type(cxt, $1, 0); }
	| float_type						{ qb_set_variable_type(cxt, $1, 0); }
	;

string_type
	: T_TYPE_STRING int_type_base		{ qb_set_variable_type(cxt, $2, QB_TYPE_DECL_STRING); }
	| T_TYPE_STRING						{ qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING); }
	;

char_type
	: T_TYPE_CHAR int_type_base			{ qb_set_variable_type(cxt, $2, QB_TYPE_DECL_STRING); }
	| T_TYPE_CHAR						{ qb_set_variable_type(cxt, QB_TYPE_U08, QB_TYPE_DECL_STRING); }
	;

boolean_type
	: T_TYPE_BOOLEAN					{ qb_set_variable_type(cxt, QB_TYPE_I32, QB_TYPE_DECL_BOOLEAN); }
	;
	
image_type
	: T_TYPE_IMAGE float_type_base		{ $$ = qb_parse_integer(cxt, $1, 10); qb_set_variable_type(cxt, $2, QB_TYPE_DECL_IMAGE);}
	| T_TYPE_IMAGE						{ $$ = qb_parse_integer(cxt, $1, 10); qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE); }
	| T_TYPE_IMAGE4 float_type_base		{ $$ = 4; qb_set_variable_type(cxt, $2, QB_TYPE_DECL_IMAGE);}
	| T_TYPE_IMAGE4						{ $$ = 4; qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_IMAGE); }
	;

vector_type
	: T_TYPE_VECTOR float_type_base		{ $$ = qb_parse_integer(cxt, $1, 10); qb_set_variable_type(cxt, $2, QB_TYPE_DECL_VECTOR); }
	| T_TYPE_VECTOR						{ $$ = qb_parse_integer(cxt, $1, 10); qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR); }
	| T_TYPE_VECTOR3 float_type_base	{ $$ = 3; qb_set_variable_type(cxt, $2, QB_TYPE_DECL_VECTOR);}
	| T_TYPE_VECTOR3					{ $$ = 3; qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_VECTOR); }
	;

complex_type
	: T_TYPE_COMPLEX float_type_base	{ qb_set_variable_type(cxt, $2, QB_TYPE_DECL_COMPLEX); }
	| T_TYPE_COMPLEX					{ qb_set_variable_type(cxt, QB_TYPE_F32, QB_TYPE_DECL_COMPLEX); }
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
	: '[' element_count ']'				{ qb_add_dimension(cxt, $2, 0); }
	| '[' ']'							{ qb_add_dimension(cxt, 0, 0); }
	| '[' '?' ']'						{ qb_add_dimension(cxt, 0, 0); }
	| '[' '*' ']'						{ qb_add_dimension(cxt, 0, QB_TYPE_DECL_AUTOVIVIFICIOUS); }
	| '[' T_LABEL ']'
	| '[' alias_scheme ']'				{ qb_add_index_alias_scheme(cxt, $2); }
	;
	
element_count
	: T_DECIMAL							{ $$ = qb_parse_integer(cxt, $1, 10); }
	| T_HEXADECIMAL						{ $$ = qb_parse_integer(cxt, $1, 16); }
	;

alias_scheme
	: name_list							{ $$ = $1; }
	| T_LABEL ':' name_list				{ $$ = $3; qb_attach_index_alias_scheme_class(cxt, $$, $1); }
	
name_list
	: T_LABEL ',' T_LABEL				{ $$ = qb_create_index_alias_scheme(cxt); qb_add_index_alias(cxt, $$, $1); qb_add_index_alias(cxt, $$, $3); }
	| name_list ',' T_LABEL				{ $$ = $1; qb_add_index_alias(cxt, $$, $3); }	
	;

%%
