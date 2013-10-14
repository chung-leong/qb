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

void qb_build(qb_build_context *cxt) {
	USE_TSRM
	qb_compiler_context *compiler_cxt;
	uint32_t i, native_compile = FALSE;

	// create the compiler contexts for all functions to be compiled 
	for(i = 0; i < cxt->function_declaration_count; i++) {
		compiler_cxt = qb_enlarge_array((void **) &cxt->compiler_contexts, 1);
		qb_initialize_compiler_context(compiler_cxt, cxt->pool, cxt->function_declarations[i] TSRMLS_CC);

		QB_G(current_filename) = compiler_cxt->zend_op_array->filename;

		// add variables used within function
		qb_add_variables(compiler_cxt);

		// set up function prototypes so the functions can resolved against each other
		qb_initialize_function_prototype(compiler_cxt);
	}

	// translate the functions
	for(i = 0; i < cxt->compiler_context_count; i++) {
		compiler_cxt = &cxt->compiler_contexts[i];

		if(!compiler_cxt->function_declaration->import_path) {
			qb_php_translater_context _translater_cxt, *translater_cxt = &_translater_cxt;
			qb_initialize_php_translater_context(translater_cxt, compiler_cxt TSRMLS_CC);

			// show the zend opcodes if turned on
			if(QB_G(show_zend_opcodes)) {
				qb_printer_context _printer_cxt, *printer_cxt = &_printer_cxt;
				qb_initialize_printer_context(printer_cxt, compiler_cxt TSRMLS_CC);
				qb_print_zend_ops(printer_cxt);
			}

			// translate the zend ops to intermediate qb ops
			qb_translate_instructions(translater_cxt);
		} else {
			// load the code into memory
			qb_load_external_code(compiler_cxt, compiler_cxt->function_declaration->import_path);

			// decode the pbj data
			//qb_pbj_decode(compiler_cxt);

			// show the pb opcodes if turned on
			if(QB_G(show_pbj_opcodes)) {
				//qb_pbj_print_ops(compiler_cxt);
			}

			// map function arguments to PB kernel parameters
			//qb_pbj_map_arguments(compiler_cxt);

			// create the main loop and translate the PB instructions
			//qb_pbj_translate_instructions(compiler_cxt);

			// free the binary
			qb_free_external_code(compiler_cxt);
		}

		// make all jump target indices absolute
		qb_resolve_jump_targets(compiler_cxt);

		// fuse basic instructions into compound ones
		qb_fuse_instructions(compiler_cxt, 1);

		// assign storage space to variables
		qb_assign_storage_space(compiler_cxt);

		// make opcodes address-mode-specific
		qb_resolve_address_modes(compiler_cxt);

		// try to fuse more instructions
		qb_fuse_instructions(compiler_cxt, 2);

		// figure out how many references to relocatable segments there are
		qb_resolve_reference_counts(compiler_cxt);

		// show the qb opcodes if turned on
		if(QB_G(show_opcodes)) {
			qb_printer_context _printer_cxt, *printer_cxt = &_printer_cxt;
			qb_initialize_printer_context(printer_cxt, compiler_cxt TSRMLS_CC);
			qb_print_ops(printer_cxt);
		}

		QB_G(current_filename) = NULL;
		QB_G(current_line_number) = 0;
	}

	// generate the instruction streams
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_encoder_context _encoder_cxt, *encoder_cxt = &_encoder_cxt;
		qb_function *qfunc;

		compiler_cxt = &cxt->compiler_contexts[i];
		qb_initialize_encoder_context(encoder_cxt, compiler_cxt TSRMLS_CC);

		// encode the instruction stream
		qfunc = qb_encode_function(encoder_cxt);

		// 
		qb_attach_compiled_function(qfunc, compiler_cxt->zend_op_array);

		if(compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE) {
			native_compile = TRUE;
		}
	}

#ifdef NATIVE_COMPILE_ENABLED
	// compile all functions inside build in one go
	if(native_compile) {
		if(QB_G(allow_native_compilation)) {
			qb_native_compile(cxt TSRMLS_C);
		}
	}
	if(!QB_G(allow_bytecode_interpretation)) {
		for(i = 0; i < cxt->compiler_context_count; i++) {
			compiler_cxt = &cxt->compiler_contexts[i];
			if(!compiler_cxt->native_proc) {
				qb_abort("unable to compile to native code");
			}
		}
	}
#endif
}

qb_class_declaration * qb_find_class_declaration(qb_build_context *cxt, zend_class_entry *ce) {
	uint32_t i = 0;
	for(i = 0; i < cxt->class_declaration_count; i++) {
		qb_class_declaration *decl = cxt->class_declarations[i];
		if(decl->zend_class == ce) {
			return decl;
		}
	}
	return NULL;
}

void qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC) {
	cxt->deferral_count = 0;
	cxt->pool = &cxt->_pool;
	qb_initialize_data_pool(cxt->pool);
	qb_attach_new_array(cxt->pool, (void **) &cxt->function_declarations, &cxt->function_declaration_count, sizeof(qb_function_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->class_declarations, &cxt->class_declaration_count, sizeof(qb_class_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->compiler_contexts, &cxt->compiler_context_count, sizeof(qb_compiler_context), 16);
	SAVE_TSRMLS
}

void qb_free_build_context(qb_build_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		qb_free_compiler_context(compiler_cxt);
	}
	qb_free_data_pool(cxt->pool);
}

