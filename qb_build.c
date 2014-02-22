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

static zend_always_inline void qb_add_function_declaration(qb_build_context *cxt, qb_function_declaration *function_decl) {
	qb_function_declaration **p = qb_enlarge_array((void **) &cxt->function_declarations, 1);
	*p = function_decl;
}

static zend_always_inline void qb_add_class_declaration(qb_build_context *cxt, qb_class_declaration *class_decl) {
	qb_class_declaration **p = qb_enlarge_array((void **) &cxt->class_declarations, 1);
	*p = class_decl;
}

static qb_class_declaration * qb_find_class_declaration(qb_build_context *cxt, zend_class_entry *ce) {
	uint32_t i = 0;
	for(i = 0; i < cxt->class_declaration_count; i++) {
		qb_class_declaration *decl = cxt->class_declarations[i];
		if(decl->zend_class == ce) {
			return decl;
		}
	}
	return NULL;
}

qb_compiler_context * qb_find_compiler_context(qb_build_context *cxt, qb_function *function_prototype) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(&compiler_cxt->function_prototype == function_prototype) {
			return compiler_cxt;
		}
	}
	return NULL;
}

static void qb_parse_declarations(qb_build_context *cxt) {
	USE_TSRM
	uint32_t i;
	for(i = 0; i < cxt->function_tag_count; i++) {
		qb_parser_context _parser_cxt, *parser_cxt = &_parser_cxt;
		qb_function_tag *tag = &cxt->function_tags[i];
		qb_function_declaration *func_decl;

		qb_initialize_parser_context(parser_cxt, cxt->pool, tag->scope, tag->op_array->filename, tag->op_array->line_start TSRMLS_CC);
		func_decl = qb_parse_function_doc_comment(parser_cxt, tag->op_array);

		if(func_decl) {
			qb_add_function_declaration(cxt, func_decl);

			if(tag->scope) {
				qb_class_declaration *class_decl = qb_find_class_declaration(cxt, tag->scope);
				if(!class_decl) {
					class_decl = qb_parse_class_doc_comment(parser_cxt, tag->scope);
					qb_add_class_declaration(cxt, class_decl);
				}
				func_decl->class_declaration = class_decl;
			}
			qb_free_parser_context(parser_cxt);
		}
	}

}

static void qb_initialize_build_environment(qb_build_context *cxt) {
	USE_TSRM
	uint32_t i;

	cxt->compiler_contexts = emalloc(sizeof(qb_compiler_context *) * cxt->function_declaration_count);
	for(i = 0; i < cxt->function_declaration_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[cxt->compiler_context_count++] = emalloc(sizeof(qb_compiler_context));
		qb_initialize_compiler_context(compiler_cxt, cxt->pool, cxt->function_declarations[i], i, cxt->function_declaration_count TSRMLS_CC);

		// add variables used within function
		if(!qb_add_variables(compiler_cxt)) {
			qb_dispatch_exceptions(TSRMLS_C);
		}

		// set up function prototypes so the functions can resolved against each other
		qb_initialize_function_prototype(compiler_cxt);

		// attach the appropriate translator
		if(!compiler_cxt->function_declaration->import_path) {
			qb_php_translator_context *translator_cxt = emalloc(sizeof(qb_php_translator_context));
			qb_initialize_php_translator_context(translator_cxt, compiler_cxt TSRMLS_CC);
			compiler_cxt->translation = QB_TRANSLATION_PHP;
			compiler_cxt->translator_context = translator_cxt;
		} else {
			qb_pbj_translator_context *translator_cxt = emalloc(sizeof(qb_pbj_translator_context));
			qb_initialize_pbj_translator_context(translator_cxt, compiler_cxt TSRMLS_CC);
			compiler_cxt->translation = QB_TRANSLATION_PBJ;
			compiler_cxt->translator_context = translator_cxt;

			// load the code into memory and decode the pbj data
			if(!qb_load_external_code(compiler_cxt, compiler_cxt->function_declaration->import_path)
			|| !qb_decode_pbj_binary(translator_cxt)) {
				qb_dispatch_exceptions(TSRMLS_C);
			}
		}

		// show the zend/pbj opcodes if turned on
		if(QB_G(show_source_opcodes)) {
			qb_printer_context _printer_cxt, *printer_cxt = &_printer_cxt;
			qb_initialize_printer_context(printer_cxt, compiler_cxt TSRMLS_CC);
			qb_print_source_ops(printer_cxt);
			qb_free_printer_context(printer_cxt);
		}
	}

	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];

		switch(compiler_cxt->translation) {
			case QB_TRANSLATION_PHP: {
				// run an initial pass over the opcode to gather info
				qb_php_translator_context *translator_cxt = compiler_cxt->translator_context;
				qb_survey_instructions(translator_cxt);
			}	break;
			case QB_TRANSLATION_PBJ: {
				qb_pbj_translator_context *translator_cxt = compiler_cxt->translator_context;
				qb_survey_pbj_instructions(translator_cxt);
			}	break;
		}
	}

	// display warning and bail out on fatal errors
	qb_dispatch_exceptions(TSRMLS_C);
}

static void qb_merge_function_dependencies(qb_build_context *cxt, qb_compiler_context *compiler_cxt, int32_t *p_changed) {
	uint32_t i, j;
	int8_t *d1 = compiler_cxt->dependencies;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		if(d1[i]) {
			// function is dependent on the other
			qb_compiler_context *other_compiler_cxt = cxt->compiler_contexts[i];
			if(compiler_cxt != other_compiler_cxt) {
				int8_t *d2 = other_compiler_cxt->dependencies;
				for(j = 0; j < cxt->compiler_context_count; j++) {
					// if the other function is dependent on yet another, this one is dependent on that too
					if(d2[j] && !d1[j]) {
						d1[j] = TRUE;
						*p_changed = TRUE;
					}
				}
			}
		}
	}
}

static void qb_reinsert_function(qb_build_context *cxt, qb_compiler_context **compiler_cxts, uint32_t index, uint32_t count) {
	uint32_t i;
	qb_compiler_context *cxt_a = compiler_cxts[index];
	for(i = 0; i < cxt->compiler_context_count; i++) {
		if(cxt->compiler_contexts[i] == cxt_a) {
			// already inserted
			return;
		}
	}
	for(i = 0; i < count; i++) {
		if(i != index) {
			qb_compiler_context *cxt_b = compiler_cxts[i];
			if(cxt_a->dependencies[cxt_b->dependency_index]) {
				if(cxt_b->function_flags & QB_FUNCTION_INLINEABLE) {
					// A calls B and B is inlineable--B needs to be inserted first
					qb_reinsert_function(cxt, compiler_cxts, i, count);
				}
			}
		}
	}
	cxt->compiler_contexts[cxt->compiler_context_count++] = cxt_a;
}

static void qb_resolve_dependencies(qb_build_context *cxt) {
	uint32_t i, j;
	if(cxt->compiler_context_count > 1) {
		int32_t changed;
		qb_compiler_context **compiler_contexts = alloca(sizeof(qb_compiler_context *) * cxt->compiler_context_count);
		uint32_t compiler_context_count = cxt->compiler_context_count;

		// keep merging until there's no change
		do {
			changed = FALSE;
			for(i = 0; i < cxt->compiler_context_count; i++) {
				qb_merge_function_dependencies(cxt, cxt->compiler_contexts[i], &changed);
			}
		} while(changed);

		for(i = 0; i < cxt->compiler_context_count; i++) {
			for(j = 0; j < cxt->compiler_context_count; j++) {
				if(i != j) {
					qb_compiler_context *cxt_a = cxt->compiler_contexts[i];
					qb_compiler_context *cxt_b = cxt->compiler_contexts[j];
					if(cxt_a->dependencies[cxt_b->dependency_index] && cxt_b->dependencies[cxt_a->dependency_index]) {
						// cross-dependency: neither one can be inlined
						cxt_a->function_flags &= ~QB_FUNCTION_INLINEABLE;
						cxt_b->function_flags &= ~QB_FUNCTION_INLINEABLE;
					}
				}
			}
		}

		memcpy(compiler_contexts, cxt->compiler_contexts, sizeof(qb_compiler_context *) * cxt->compiler_context_count);
		cxt->compiler_context_count = 0;
		for(i = 0; i < compiler_context_count; i++) {
			qb_reinsert_function(cxt, compiler_contexts, i, compiler_context_count);
		}
	}
}

void qb_perform_translation(qb_build_context *cxt) {
	USE_TSRM
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];

		switch(compiler_cxt->translation) {
			case QB_TRANSLATION_PHP: {
				qb_php_translator_context *translator_cxt = compiler_cxt->translator_context;
				// translate the zend ops to intermediate qb ops
				qb_translate_instructions(translator_cxt);
			}	break;
			case QB_TRANSLATION_PBJ: {
				qb_pbj_translator_context *translator_cxt = compiler_cxt->translator_context;
				// create the main loop and translate the PB instructions
				qb_translate_pbj_instructions(translator_cxt);

				// free the binary
				qb_free_external_code(compiler_cxt);
			}	break;
		}

		// display warning and bail out on fatal errors
		qb_dispatch_exceptions(TSRMLS_C);

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
	}
}

void qb_generate_executables(qb_build_context *cxt) {
	USE_TSRM
	int32_t native_compile = FALSE;
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		qb_encoder_context _encoder_cxt, *encoder_cxt = &_encoder_cxt;

		qb_initialize_encoder_context(encoder_cxt, compiler_cxt, TRUE TSRMLS_CC);

		// encode the instruction stream
		compiler_cxt->compiled_function = qb_encode_function(encoder_cxt);

		if(!compiler_cxt->compiled_function) {
			qb_dispatch_exceptions(TSRMLS_C);
		}

		// relocate the function now, so the base function won't be in the middle of relcoation while it's being copied
		qb_relocate_function(compiler_cxt->compiled_function, TRUE);

		// attach the function to the op array
		qb_attach_compiled_function(compiler_cxt->compiled_function, compiler_cxt->zend_op_array TSRMLS_CC);

		if(compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE) {
			native_compile = TRUE;
		}
	}

#ifdef NATIVE_COMPILE_ENABLED
	// compile all functions inside build in one go
	if(native_compile) {
		if(QB_G(allow_native_compilation)) {
			qb_native_compiler_context _native_compiler_cxt, *native_compiler_cxt = &_native_compiler_cxt;
			qb_initialize_native_compiler_context(native_compiler_cxt, cxt TSRMLS_CC);
			qb_compile_to_native_code(native_compiler_cxt);
			qb_free_native_compiler_context(native_compiler_cxt);
		}
	}
	if(!QB_G(allow_bytecode_interpretation)) {
		for(i = 0; i < cxt->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
			if(!compiler_cxt->compiled_function->native_proc) {
				qb_report_native_compilation_exception(compiler_cxt->compiled_function->line_id, compiler_cxt->compiled_function->name);
				qb_dispatch_exceptions(TSRMLS_C);
			}
		}
	}
#endif
}

void qb_build(qb_build_context *cxt) {
	// parse the doc comments
	qb_parse_declarations(cxt);

	// create the compiler contexts for all functions to be compiled 
	qb_initialize_build_environment(cxt);

	// resolve function dependencies
	qb_resolve_dependencies(cxt);

	// translate the functions
	qb_perform_translation(cxt);

	// generate the instruction streams
	qb_generate_executables(cxt);
}

void qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC) {
	cxt->pool = &cxt->_pool;
	cxt->compiler_contexts = NULL;
	cxt->compiler_context_count = 0;
	qb_initialize_data_pool(cxt->pool);
	qb_attach_new_array(cxt->pool, (void **) &cxt->function_tags, &cxt->function_tag_count, sizeof(qb_function_tag), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->function_declarations, &cxt->function_declaration_count, sizeof(qb_function_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->class_declarations, &cxt->class_declaration_count, sizeof(qb_class_declaration *), 16);
	SAVE_TSRMLS
}

void qb_free_build_context(qb_build_context *cxt) {
	qb_free_data_pool(cxt->pool);

	if(cxt->compiler_contexts) {
		uint32_t i;
		for(i = 0; i < cxt->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
			if(compiler_cxt->translator_context) {
				switch(compiler_cxt->translation) {
					case QB_TRANSLATION_PHP: {
						qb_free_php_translator_context(compiler_cxt->translator_context);
					}	break;
					case QB_TRANSLATION_PBJ: {
						qb_free_pbj_translator_context(compiler_cxt->translator_context);
					}	break;
				}
				efree(compiler_cxt->translator_context);
			}
			qb_free_compiler_context(compiler_cxt);
			efree(compiler_cxt);
		}
		efree(cxt->compiler_contexts);
	}
}

