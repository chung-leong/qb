#include "qb.h"
#include "qb_parser_re2c.h"
#include "qb_parser_bison.h"

#define YYCTYPE				char
#define YYCURSOR 			l->cursor
#define YYMARKER			l->marker
#define YYCONDTYPE			int
#define YYGETCONDITION()	l->condition
#define YYSETCONDITION(c)	l->condition = c

int qb_scan_next_token(qb_lexer_context *l) {

skip:
	l->token = l->cursor; 

/*!re2c

	re2c:yyfill:enable   = 0;
	re2c:yych:conversion = 1;
	re2c:indent:top      = 1;
	
	LABEL = [a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*;
	WS = [ \t];
	NEWLINE = [\r\n];
	NON_WS = [^ \t\r\n\x00]+;
	TERMINATOR = "\000";
	POS_INT = [1-9][0-9]*;
	DECIMAL = [0-9]+;
	HEXADECIMAL	= "0x"[0-9a-fA-F]+;
	ANYTHING = [^\x00\r\n]+;
	
	<INITIAL> "@engine" => PARAM			{ return T_TAG_ENGINE; }
	
	<PARAM> "qb"							{ return T_ENGINE_QB; }
	
	<PARAM> "qb-native"						{ return T_ENGINE_QB_NATIVE; }

	<PARAM> "qb-bytecode"					{ return T_ENGINE_QB_BYTECODE; }
	
	<INITIAL> "@inline" => PARAM			{ return T_TAG_ENGINE; }

	<PARAM> "always"						{ return T_ALWAYS; }
	
	<PARAM> "never"							{ return T_NEVER; }
	
	<PARAM> ["] => PARAM_DBL_QUOTES			{ goto skip; }

	<PARAM_DBL_QUOTES>["] => PARAM			{ goto skip; }
	
	<PARAM_DBL_QUOTES> [^"\r\n]+			{ return T_LABEL; }
	
	<PARAM_DBL_QUOTES> NEWLINE				{ return T_UNEXPECTED; }

	<PARAM> ['] => PARAM_SG_QUOTES			{ goto skip; }

	<PARAM_SG_QUOTES>['] => PARAM			{ goto skip; }
	
	<PARAM_SG_QUOTES> [^'\r\n]+				{ return T_LABEL; }
	
	<PARAM_SG_QUOTES> NEWLINE				{ return T_UNEXPECTED; }
	
	<INITIAL> "@global" => TYPE_DECL		{ return T_TAG_GLOBAL; }
	
	<INITIAL> "@local" => TYPE_DECL			{ return T_TAG_LOCAL; }
	
	<INITIAL> "@shared" => TYPE_DECL		{ return T_TAG_SHARED; }
	
	<INITIAL> "@lexical" => TYPE_DECL		{ return T_TAG_LEXICAL; }

	<INITIAL> "@param" => TYPE_DECL			{ return T_TAG_PARAM; }
	
	<INITIAL> "@static" => TYPE_DECL		{ return T_TAG_STATIC; }

	<INITIAL> "@property" => TYPE_DECL		{ return T_TAG_PROPERTY; }
	
	<INITIAL> "@var" => TYPE_DECL			{ return T_TAG_VAR; }
	
	<INITIAL> "@return" => TYPE_DECL		{ return T_TAG_RETURN; }
	
	<INITIAL> "@receive" => TYPE_DECL		{ return T_TAG_RECEIVE; }
	
	<TYPE_DECL> ("int"|"integer")			{ return T_TYPE_INT; }
	
	<TYPE_DECL> "int8"						{ return T_TYPE_INT08; }
	
	<TYPE_DECL> "int16"						{ return T_TYPE_INT16; }
	
	<TYPE_DECL> "int32"						{ return T_TYPE_INT32; }
	
	<TYPE_DECL> "int64"						{ return T_TYPE_INT64; }
	
	<TYPE_DECL> "uint8"						{ return T_TYPE_UINT08; }
	
	<TYPE_DECL> "uint16"					{ return T_TYPE_UINT16; }
	
	<TYPE_DECL> "uint32"					{ return T_TYPE_UINT32; }
	
	<TYPE_DECL> "uint64"					{ return T_TYPE_UINT64;	}
	
	<TYPE_DECL> "float"						{ return T_TYPE_FLOAT; }
	
	<TYPE_DECL> "double"					{ return T_TYPE_DOUBLE; }
	
	<TYPE_DECL> "float32"					{ return T_TYPE_FLOAT32; }
	
	<TYPE_DECL> "float64"					{ return T_TYPE_FLOAT64; }
	
	<TYPE_DECL> "void"						{ return T_TYPE_VOID; }
	
	<TYPE_DECL> "string"					{ return T_TYPE_STRING; }
	
	<TYPE_DECL> "char"						{ return T_TYPE_CHAR; }
	
	<TYPE_DECL> ("bool"|"boolean")			{ return T_TYPE_BOOLEAN; }
	
	<TYPE_DECL> "image"[1-4]				{ return T_TYPE_IMAGE; }
	
	<TYPE_DECL> "image"						{ return T_TYPE_IMAGE4; }

	<TYPE_DECL> "complex"					{ return T_TYPE_COMPLEX; }
	
	<TYPE_DECL> "vector"POS_INT				{ return T_TYPE_VECTOR; }
	
	<TYPE_DECL> "vector"					{ return T_TYPE_VECTOR3; }
	
	<TYPE_DECL> "matrix"POS_INT"x"POS_INT	{ return T_TYPE_MATRIX; }
	
	<TYPE_DECL> "matrix"					{ return T_TYPE_MATRIX3X3; }
	
	<TYPE_DECL> "["							{ return '['; }
	
	<TYPE_DECL> "]"							{ return ']'; }
	
	<TYPE_DECL> "<"							{ return '<'; }
	
	<TYPE_DECL> ">"							{ return '>'; }
	
	<TYPE_DECL> "*"							{ return '*'; }
	
	<TYPE_DECL> "?"							{ return '?'; }

	<TYPE_DECL> ":"							{ return ':'; }
	
	<TYPE_DECL> ","							{ return ','; }
	
	<TYPE_DECL> DECIMAL						{ return T_DECIMAL; }
	
	<TYPE_DECL> HEXADECIMAL					{ return T_HEXADECIMAL; }
	
	<TYPE_DECL> LABEL						{ return T_LABEL; }
	
	<TYPE_DECL> "$" => VARIABLE_NAME		{ return '$'; }
	
	/* if it's a valid variable name */
	<VARIABLE_NAME> LABEL => TYPE_DECL		{ return T_LABEL; }
	
	/* assume it's a regexp if it doesn't match LABEL */
	<VARIABLE_NAME> NON_WS => TYPE_DECL		{ return T_REGEXP; }
	
	/* don't allow whitespace after the $ */
	<VARIABLE_NAME> WS						{ return T_UNEXPECTED; }
	
	<INITIAL> "*"							{ goto skip; }
	
	<INITIAL> "/**"							{ goto skip; }		
	
	<INITIAL> NEWLINE						{ return T_COMMENT; }
	
	/* if there's no match, then the line is a comment */
	<INITIAL> NON_WS :=> COMMENT
	
	<COMMENT> ANYTHING						{ return T_COMMENT; }
	
	<COMMENT> NEWLINE => INITIAL			{ return T_COMMENT; }
	
	<*> TERMINATOR							{ return 0; }
	
	<*> WS {
		if(l->token + 1 == l->cursor) {
			// just white space--jump over it
			goto skip;
		} else {
			// there was something there that didn't match anything
			return T_UNEXPECTED;
		}
	}
*/
}
