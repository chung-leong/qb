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

#if NATIVE_COMPILE_ENABLED

#ifdef __GNUC__
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/wait.h>
	#include <unistd.h>
	#include <fcntl.h>
#endif

#ifdef __ELF__
	#include <elf.h>
#endif

#ifdef __MACH__
	#include <mach-o/loader.h>
	#include <mach-o/reloc.h>
	#include <mach-o/nlist.h>
	#include <mach-o/stab.h>
	#include <mach-o/x86_64/reloc.h>
#endif

// note: fastcall could cause a variable to fall off the list on OSX
ZEND_ATTRIBUTE_FORMAT(printf, 2, 3)
static void qb_printf(qb_native_compiler_context *cxt, const char *format, ...) {
	USE_TSRM
	va_list args;
	va_start(args, format);
	vfprintf(cxt->write_stream, format, args);
	va_end(args);

	if(cxt->print_source) {
		char *buffer;
		int len;
		va_start(args, format);
		len = vspprintf(&buffer, 0, format, args);
		php_write(buffer, len TSRMLS_CC);
		efree(buffer);
	}
}

static void ZEND_FASTCALL qb_print(qb_native_compiler_context *cxt, const char *s) {
	USE_TSRM
	uint32_t len = strlen(s);
	if(cxt->print_source) {
		php_write((char *) s, len TSRMLS_CC);
	}
	fwrite(s, 1, len, cxt->write_stream);
}

#ifdef __GNUC__
static int32_t ZEND_FASTCALL qb_launch_gcc(qb_native_compiler_context *cxt) {
	USE_TSRM

	int gcc_pipe_write[2];
	int gcc_pipe_read[2];
	int gcc_pipe_error[2];
	pipe(gcc_pipe_read);
	pipe(gcc_pipe_write);
	pipe(gcc_pipe_error);

	const char *compiler_path = QB_G(compiler_path);
	const char *compiler_env_path = QB_G(compiler_env_path);

	pid_t pid = fork();
	if(pid == 0) {
		// set up stdin, stdout, and stderr
		dup2(gcc_pipe_write[0], STDIN_FILENO);
		dup2(gcc_pipe_read[1], STDOUT_FILENO);
		dup2(gcc_pipe_error[1], STDERR_FILENO);
		close(gcc_pipe_write[0]);
		close(gcc_pipe_write[1]);
		close(gcc_pipe_read[0]);
		close(gcc_pipe_read[1]);
		close(gcc_pipe_error[0]);
		close(gcc_pipe_error[1]);

		// start gcc
		const char *args[16];
		int argc = 0;
		if(strlen(compiler_path) > 0) {
			args[argc++] = compiler_path;
		} else {
			args[argc++] = "c99";
		}
		args[argc++] = "-c";
		args[argc++] = "-O2";										// optimization level
#ifdef HAVE_GCC_MARCH_NATIVE
		args[argc++] = "-march=native";								// optimize for current CPU
#endif
		args[argc++] = "-pipe";										// use pipes for internal communication
#if !ZEND_DEBUG
		args[argc++] = "-Wp,-w";									// disable preprocessor warning
#endif
		args[argc++] = "-Werror=implicit-function-declaration";		// elevate implicit function declaration to an error
		args[argc++] = "-fno-stack-protector"; 						// disable stack protector
#ifdef __LP64__
		args[argc++] = "-mcmodel=large";							// use large memory model, since qb extension could be anywhere in the address space
#endif
		args[argc++] = "-o";
		args[argc++] = cxt->obj_file_path;
		args[argc++] = "-xc";										// indicate the source is C
		args[argc++] = "-";											// input from stdin
		args[argc++] = NULL;

		if(strlen(compiler_env_path) > 0) {
			setenv("PATH", compiler_env_path, TRUE);
		}
		execvp("gcc", (char **) args);
		_exit(255);
	}

	close(gcc_pipe_write[0]);
	close(gcc_pipe_read[1]);
	close(gcc_pipe_error[1]);

	cxt->write_stream = fdopen(gcc_pipe_write[1], "w");
	cxt->read_stream = fdopen(gcc_pipe_read[0], "r");
	cxt->error_stream = fdopen(gcc_pipe_error[0], "r");

	return (pid > 0 && cxt->write_stream && cxt->read_stream && cxt->error_stream);
}
#endif	// __GNUC__

#ifdef _MSC_VER
static int32_t ZEND_FASTCALL qb_launch_cl(qb_native_compiler_context *cxt) {
	USE_TSRM
	const char *compiler_path = QB_G(compiler_path);
	const char *compiler_env_path = QB_G(compiler_env_path);
	const char *sse_option;
	HANDLE c_file;
	char *command_line;
	char *path_before = NULL;
	UINT error_mode_before;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	int cpu_flags[4], file_descriptor;
	HANDLE pipe_error_write, pipe_err_read;

	// open temporary c file for writing (Visual C doesn't accept input from stdin)
	spprintf(&cxt->c_file_path, 0, "%s%cQB%" PRIX64 ".c", cxt->cache_folder_path, PHP_DIR_SEPARATOR, cxt->file_id);
	c_file = CreateFile(cxt->c_file_path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(c_file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	file_descriptor = _open_osfhandle((intptr_t) c_file, 0);
	cxt->write_stream = _fdopen(file_descriptor, "w");

	// open a pipe for stderr
	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	if(!CreatePipe(&pipe_err_read, &pipe_error_write, &sa, 0)) {
		return FALSE;
	}
	file_descriptor = _open_osfhandle((intptr_t) pipe_err_read, 0);
	cxt->error_stream = _fdopen(file_descriptor, "r");

	if(strlen(compiler_path) == 0) {
		compiler_path = "cl";
	}
	if(strlen(compiler_env_path) != 0) {
		// save the current value of PATH
		uint32_t path_before_length = GetEnvironmentVariable("PATH", NULL, 0);
		path_before = emalloc(path_before_length);
		GetEnvironmentVariable("PATH", path_before, path_before_length);
		SetEnvironmentVariable("PATH", compiler_env_path);
	}

	// stop the appearance of pop up warnings
	error_mode_before = SetErrorMode(0);

	// see if the CPU supports SSE
	__cpuid(cpu_flags, 0x00000001);
	if(cpu_flags[2] & (1 << 20) || cpu_flags[2] & (1 << 19) || cpu_flags[2] & (1 << 0) || cpu_flags[3] & (1 << 26)) {
		sse_option = "/arch:SSE2";
	} else if(cpu_flags[3] & (1 << 25)) {
		sse_option = "/arch:SSE";
	} else {
		sse_option = "";
	}

	// /O2		maximize speed
	// /Oy		enable frame pointer omission
	// /GS-		disable buffer security check
	// /GF		enable read-only string pooling
	// /c		compile without linking
	// /w		disable all warnings
	// /nologo	suppress startup banner
#ifdef ZEND_DEBUG
	spprintf(&command_line, 0, "\"%s\" /O2 /Oy /GS- /w /fp:precise %s /nologo /Fo\"%s\" /c \"%s\"", compiler_path, sse_option, cxt->obj_file_path, cxt->c_file_path);
#else
	//spprintf(&command_line, 0, "\"%s\" /O2 /Oy /GS- /fp:precise %s /nologo /Fo\"%s\" /c \"%s\"", compiler_path, sse_option, cxt->obj_file_path, cxt->c_file_path);
#endif

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;

	// cl.exe outputs compilation errors to stdout, so we need to capture both stderr and stdout
	si.hStdOutput = pipe_error_write;
	si.hStdError = pipe_error_write;
	if(CreateProcess(NULL, command_line, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		cxt->msc_thread = pi.hThread;
		cxt->msc_process = pi.hProcess;
	}
	CloseHandle(pipe_error_write);
	efree(command_line);

	if(path_before) {
		// restore PATH
		SetEnvironmentVariable("PATH", path_before);
		efree(path_before);
	}
	SetErrorMode(error_mode_before);
	return (cxt->msc_process != NULL);
}
#endif

static zend_always_inline int32_t qb_launch_compiler(qb_native_compiler_context *cxt) {
#ifdef __GNUC__
	return qb_launch_gcc(cxt);
#endif
#ifdef _MSC_VER
	return qb_launch_cl(cxt);
#endif
}

static void ZEND_FASTCALL qb_print_macros(qb_native_compiler_context *cxt) {
	qb_print(cxt, "#define NAN	"STRING(NAN)"\n");
	qb_print(cxt, "#define INF	"STRING(INFINITY)"\n");
	qb_print(cxt, "#define restrict	__restrict\n");
	qb_print(cxt, "#define NULL	"STRING(NULL)"\n");
	qb_print(cxt, "#define M_PI	"STRING(M_PI)"\n");
	qb_print(cxt, "#define MAX_DIMENSION		64\n");
#ifdef QB_LITTLE_ENDIAN
	qb_print(cxt, "#define QB_LITTLE_ENDIAN\n");
#else
	qb_print(cxt, "#define QB_BIG_ENDIAN\n");
#endif
	qb_print(cxt, "#define QB_STORAGE_STATIC_INITIALIZED	0x0002\n");
	qb_print(cxt, "#define TSRMLS_C		"STRING(TSRMLS_C)"\n");
	qb_print(cxt, "#define TSRMLS_CC	"STRING(TSRMLS_CC)"\n");
	qb_print(cxt, "#define TSRMLS_D		"STRING(TSRMLS_D)"\n");
	qb_print(cxt, "#define TSRMLS_DC	"STRING(TSRMLS_DC)"\n");
	qb_print(cxt, "#define PRId8	"STRING(PRId8)"\n");
	qb_print(cxt, "#define PRId16	"STRING(PRId16)"\n");
	qb_print(cxt, "#define PRId32	"STRING(PRId32)"\n");
	qb_print(cxt, "#define PRId64	"STRING(PRId64)"\n");
	qb_print(cxt, "#define PRIu8	"STRING(PRIu8)"\n");
	qb_print(cxt, "#define PRIu16	"STRING(PRIu16)"\n");
	qb_print(cxt, "#define PRIu32	"STRING(PRIu32)"\n");
	qb_print(cxt, "#define PRIu64	"STRING(PRIu64)"\n");
	qb_print(cxt, "#define NO_RETURN	"STRING(NO_RETURN)"\n");
	qb_print(cxt, "#define UNEXPECTED(c)	"STRING(UNEXPECTED(c))"\n");
#ifndef _MSC_VER
	qb_print(cxt, "#define ZEND_FASTCALL	"STRING(ZEND_FASTCALL)"\n");
#endif
	qb_print(cxt, "#define zend_always_inline	"STRING(zend_always_inline)"\n");

	qb_print(cxt, "#define SWAP_BE_I16(v)	"STRING(SWAP_BE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I32(v)	"STRING(SWAP_BE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I64(v)	"STRING(SWAP_BE_I64(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I16(v)	"STRING(SWAP_LE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I32(v)	"STRING(SWAP_LE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I64(v)	"STRING(SWAP_LE_I64(v))"\n");

#ifdef __GNUC__
#ifndef __builtin_bswap16
	qb_print(cxt, "#define __builtin_bswap16(n)	((n >> 8) | (n << 8))\n");
#endif
#endif
}

static void ZEND_FASTCALL qb_print_typedefs(qb_native_compiler_context *cxt) {
#ifdef __GNUC__
	qb_print(cxt, STRING(typedef __SIZE_TYPE__ size_t;)"\n");
#endif
#ifdef _MSC_VER
	#ifdef  _WIN64
	qb_print(cxt, "typedef unsigned __int64 size_t;\n");
	#else
	qb_print(cxt, "typedef unsigned int size_t;\n");
	#endif
#endif
	qb_print(cxt, "typedef char int8_t;\n");
	qb_print(cxt, "typedef unsigned char uint8_t;\n");
	qb_print(cxt, "typedef short int16_t;\n");
	qb_print(cxt, "typedef unsigned short uint16_t;\n");
	qb_print(cxt, "typedef int int32_t;\n");
	qb_print(cxt, "typedef unsigned int uint32_t;\n");
#if SIZEOF_LONG == 8
	qb_print(cxt, "typedef long int64_t;\n");
	qb_print(cxt, "typedef unsigned long uint64_t;\n");
#else
	qb_print(cxt, "typedef long long int64_t;\n");
	qb_print(cxt, "typedef unsigned long long uint64_t;\n");
#endif
	qb_print(cxt, "typedef float float32_t;\n");
	qb_print(cxt, "typedef double float64_t;\n");
	qb_print(cxt, "\n");

	qb_print(cxt, "typedef struct qb_address\tqb_address;\n");
	qb_print(cxt, "typedef struct qb_variable\tqb_variable;\n");
	qb_print(cxt, "typedef struct qb_variable_import\tqb_variable_import;\n");
	qb_print(cxt, "typedef struct qb_call_stack_item\tqb_call_stack_item;\n");
	qb_print(cxt, "typedef struct zend_function\tzend_function;\n");
	qb_print(cxt, "typedef struct zval\tzval;\n");
	qb_print(cxt, "typedef struct php_stream\tphp_stream;\n");
	qb_print(cxt, "typedef struct qb_index_alias_scheme\tqb_index_alias_scheme;\n");
	qb_print(cxt, "typedef struct qb_memory_segment\tqb_memory_segment;\n");
	qb_print(cxt, "typedef struct qb_storage\tqb_storage;\n");
	qb_print(cxt, "typedef struct qb_function\tqb_function;\n");
	qb_print(cxt, "typedef struct qb_interpreter_context\tqb_interpreter_context;\n");
	qb_print(cxt, "typedef struct qb_external_symbol\tqb_external_symbol;\n");
	qb_print(cxt, "typedef struct qb_complex_F64\tqb_complex_F64;\n");
	qb_print(cxt, "typedef struct qb_complex_F32\tqb_complex_F32;\n");

	qb_print(cxt, "\
struct qb_complex_F32 {\
	float32_t r;\
	float32_t i;\
};\
\n");

	qb_print(cxt, "\
struct qb_complex_F64 {\
	float64_t r;\
	float64_t i;\
};\
\n");

	qb_print(cxt, "\
struct qb_address {\
	uint32_t mode;\
	uint32_t type;\
	uint32_t flags;\
	uint32_t segment_selector;\
	uint32_t segment_offset;\
	qb_address *array_index_address;\
	qb_address *array_size_address;\
	uint32_t dimension_count;\
	qb_address **dimension_addresses;\
	qb_address **array_size_addresses;\
	qb_address *source_address;\
	qb_index_alias_scheme **index_alias_schemes;\
};\
\n");

	qb_print(cxt, "\
struct qb_memory_segment {\
	int8_t *memory;\
	uint32_t flags;\
	uint32_t type;\
	uint32_t element_count;\
	uint32_t current_allocation;\
	uint32_t *array_size_pointer;\
	uint32_t *dimension_pointer;\
	uint32_t *increment_pointer;\
	php_stream *stream;\
	int8_t *restrict *stack_ref_memory;\
	uint32_t *stack_ref_element_count;\
};\
\n");

	qb_print(cxt, "\
struct qb_storage {\
	qb_memory_segment *segments;\
	uint32_t segment_count;\
	uint32_t size;\
	uint32_t flags;\
};\
\n");

	qb_print(cxt, "\
struct qb_external_symbol {\
	uint32_t type;\
	const char *name;\
	uint32_t name_length;\
	void *pointer;\
};\
\n");

	qb_print(cxt, "\
struct qb_function {\
	int8_t *instructions;\
	uint32_t instruction_length;\
	uint64_t instruction_crc64;\
	uint32_t flags;\
	qb_variable *return_variable;\
	qb_variable **variables;\
	uint32_t variable_count;\
	uint32_t argument_count;\
	uint32_t required_argument_count;\
	qb_external_symbol **external_symbols;\
	uint32_t external_symbol_count;\
	qb_storage *local_storage;\
	const char *name;\
	const char *filename;\
	void *native_proc;\
	zend_function *zend_function;\
};\
\n");

	qb_print(cxt, "\
struct qb_interpreter_context {\
	qb_storage *storage;\
	qb_function *function;\
	uint32_t flags;\
	qb_call_stack_item *call_stack;\
	uint32_t call_stack_height;\
	qb_variable_import *variable_imports;\
	uint32_t variable_import_count;\
	int32_t exception_encountered;\
	uint32_t function_call_line_number;\
	uint32_t *line_number_pointer;\
	volatile unsigned char *windows_timed_out_pointer;\
	int floating_point_precision;\
	void ***tsrm_ls;\
	zend_function *zend_function;\
	uint32_t argument_count;\
	zval **zend_arguments;\
	zval ***zend_argument_pointers;\
	uint32_t zend_argument_buffer_size;\
	zval *this_object;\
	qb_address *argument_address;\
	qb_address value_address;\
	qb_address array_address;\
	qb_address *dimension_addresse_pointers[MAX_DIMENSION];\
	qb_address *array_size_address_pointers[MAX_DIMENSION];\
	qb_address dimension_addresses[MAX_DIMENSION];\
	qb_address array_size_addresses[MAX_DIMENSION];\
};\
\n");

#if ZEND_DEBUG
	qb_print(cxt, "\
typedef struct qb_native_proc_record {\
	uint64_t crc64;\
	void *proc;\
} qb_native_proc_record;\
\n");
#endif

	qb_print(cxt, "\n");
}

#define PROTOTYPE_COUNT		500

static void ZEND_FASTCALL qb_print_prototypes(qb_native_compiler_context *cxt) {
	uint32_t i, j, k;
	int32_t *prototype_indices, index;
	uint32_t printed[PROTOTYPE_COUNT];
	qb_op *qop;
	memset(printed, 0, sizeof(printed));
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		if(!compiler_cxt->native_proc && (compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE)) {
			// go through all ops and see what functions they use
			for(j = 0; j < compiler_cxt->op_count; j++) {
				qop = compiler_cxt->ops[j];
				prototype_indices = cxt->op_function_usages[qop->opcode];
				for(k = 0; prototype_indices[k] != 0xFFFFFFFF; k++) {
					index = prototype_indices[k];
					if(!printed[index]) {
						const char *prototype = cxt->function_prototypes[index];
						qb_print(cxt, prototype);
						qb_print(cxt, "\n");
						printed[index] = TRUE;
					}
				}
			}

		}
	}
	qb_print(cxt, "\n");
}

static char * ZEND_FASTCALL qb_get_buffer(qb_native_compiler_context *cxt) {
	if(cxt->string_buffer_index == 16) {
		cxt->string_buffer_index = 0;
	}
	return cxt->string_buffers[cxt->string_buffer_index++];
}

static qb_address * ZEND_FASTCALL qb_get_root_container(qb_native_compiler_context *cxt, qb_address *address) {
	while(address->source_address) {
		address = address->source_address;
	}
	return address;
}

static const char * ZEND_FASTCALL qb_get_scalar(qb_native_compiler_context *cxt, qb_address *address);
static const char * ZEND_FASTCALL qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address);
static const char * ZEND_FASTCALL qb_get_segment_pointer(qb_native_compiler_context *cxt, qb_address *address);

static const char * ZEND_FASTCALL qb_get_scalar(qb_native_compiler_context *cxt, qb_address *address) {
	char *buffer = qb_get_buffer(cxt);
	if(address->flags & QB_ADDRESS_CONSTANT) {
		switch(address->type) {
			case QB_TYPE_S08: snprintf(buffer, 128, "%" PRId8, VALUE(S08, address)); break;
			case QB_TYPE_U08: snprintf(buffer, 128, "%" PRIu8"U", VALUE(U08, address)); break;
			case QB_TYPE_S16: snprintf(buffer, 128, "%" PRId16, VALUE(S16, address)); break;
			case QB_TYPE_U16: snprintf(buffer, 128, "%" PRIu16"U", VALUE(U16, address)); break;
			case QB_TYPE_S32: snprintf(buffer, 128, "%" PRId32, VALUE(S32, address)); break;
			case QB_TYPE_U32: snprintf(buffer, 128, "%" PRIu32"U", VALUE(U32, address)); break;
			case QB_TYPE_S64: snprintf(buffer, 128, "%" PRId64"LL", VALUE(S64, address)); break;
			case QB_TYPE_U64: snprintf(buffer, 128, "%" PRIu64"ULL", VALUE(U64, address)); break;
			case QB_TYPE_F32: snprintf(buffer, 128, "%.11G", VALUE(F32, address)); break;
			case QB_TYPE_F64: snprintf(buffer, 128, "%.17G", VALUE(F64, address)); break;
			default: break;
		}
	} else if(IS_ARRAY_MEMBER(address)) {
		snprintf(buffer, 128, "%s[0]", qb_get_segment_pointer(cxt, address));
	} else if(IS_CAST(address)) {
		const char *ctype = type_cnames[address->type];
		snprintf(buffer, 128, "((%s) %s)", ctype, qb_get_scalar(cxt, address->source_address));
	} else {
		const char *type = type_names[address->type];
		uint32_t id = address->segment_offset / 4;
		snprintf(buffer, 128, "var_%s_%d", type, id);
	}
	return buffer;
}

static const char * ZEND_FASTCALL qb_get_segment_pointer(qb_native_compiler_context *cxt, qb_address *address) {
	const char *ctype = type_cnames[address->type];
	char *buffer = qb_get_buffer(cxt);
	if(address->array_index_address) {
		// note: the cast is applied to the pointer first, and then the index is added
		snprintf(buffer, 128, "((%s *) segment%d + %s)", ctype, address->segment_selector, qb_get_scalar(cxt, address->array_index_address));
	} else if(address->segment_offset) {
		// note: the offset is added before the cast is applied
		snprintf(buffer, 128, "((%s *) (segment%d + %d))", ctype, address->segment_selector, address->segment_offset);
	} else {
		snprintf(buffer, 128, "((%s *) segment%d)", ctype, address->segment_selector);
	}
	return buffer;
}

static const char * ZEND_FASTCALL qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address) {
	if(IS_SCALAR(address) && !(address->flags & QB_ADDRESS_CONSTANT)) {
		char *buffer = qb_get_buffer(cxt);
		if(IS_CAST(address)) {
			const char *ctype = type_cnames[address->type];
			snprintf(buffer, 128, "((%s *) &%s)", ctype, qb_get_scalar(cxt, address->source_address));
		} else {
			snprintf(buffer, 128, "&%s", qb_get_scalar(cxt, address));
		}
		return buffer;
	} else {
		return qb_get_segment_pointer(cxt, address);
	}
}

static const char * ZEND_FASTCALL qb_get_array_size(qb_native_compiler_context *cxt, qb_address *address) {
	if(address->array_size_address) {
		return qb_get_scalar(cxt, address->array_size_address);
	} else {
		return "1";
	}
}

static const char * ZEND_FASTCALL qb_get_segment_index(qb_native_compiler_context *cxt, qb_address *address) {
	if(address->array_index_address) {
		return qb_get_scalar(cxt, address->array_index_address);
	} else if(address->segment_offset) {
		char *buffer = qb_get_buffer(cxt);
		snprintf(buffer, 128, "%d", ELEMENT_COUNT(address->segment_offset, address->type));
		return buffer;
	} else {
		return "0";
	}
}

static const char * ZEND_FASTCALL qb_get_segment_offset(qb_native_compiler_context *cxt, qb_address *address) {
	if(address->array_index_address) {
		char *buffer = qb_get_buffer(cxt);
		snprintf(buffer, 128, "%s << %d", qb_get_scalar(cxt, address->array_index_address), type_size_shifts[address->type]);
		return buffer;
	} else if(address->segment_offset) {
		char *buffer = qb_get_buffer(cxt);
		snprintf(buffer, 128, "%d", address->segment_offset);
		return buffer;
	} else {
		return "0";
	}
}

static int32_t qb_get_jump_direction(qb_native_compiler_context *cxt, uint32_t jump_target_index, uint32_t current_qop_index) {
	if(jump_target_index != QB_INSTRUCTION_NEXT) {
		uint32_t target_qop_index;
		if(jump_target_index & QB_INSTRUCTION_OFFSET) {
			target_qop_index = current_qop_index + (jump_target_index & ~QB_INSTRUCTION_OFFSET);
		} else {
			target_qop_index = cxt->op_translations[jump_target_index];
		}
		if(target_qop_index <= current_qop_index) {
			return -1;
		}
	}
	return +1;
}

static const char * ZEND_FASTCALL qb_get_jump(qb_native_compiler_context *cxt, uint32_t jump_target_index, uint32_t current_qop_index) {
	if(jump_target_index != QB_INSTRUCTION_NEXT) {
		uint32_t target_qop_index, next_qop_index;
		if(jump_target_index & QB_INSTRUCTION_OFFSET) {
			target_qop_index = current_qop_index + (jump_target_index & ~QB_INSTRUCTION_OFFSET);
		} else {
			target_qop_index = cxt->op_translations[jump_target_index];
		}
		// return the goto only if the location isn't the next op
		next_qop_index = current_qop_index + 1;
		while(cxt->ops[next_qop_index]->opcode == QB_NOP) {
			next_qop_index++;
		}
		if(target_qop_index != next_qop_index) {
			char *buffer = qb_get_buffer(cxt);
			snprintf(buffer, 128, "goto L%04d", target_qop_index);
			return buffer;
		}
	}
	return NULL;
}

static int32_t ZEND_FASTCALL qb_is_always_in_bound(qb_native_compiler_context *cxt, qb_address *container, qb_address *address, uint32_t new_element_count) {
	if(container != address || new_element_count != 0) {
		uint32_t index, element_count, container_size;
		if(IS_SCALAR(address)) {
			element_count = 1;
		} else {
			if(address->array_size_address->flags & QB_ADDRESS_CONSTANT) {
				// sub-array size is not constant
				if(address->flags & QB_ADDRESS_ALWAYS_IN_BOUND) {
					// an index set by foreach(), so it's always going to be in bound
					return TRUE;
				} else {
					return FALSE;
				}
			} else {
				element_count = ARRAY_SIZE(address);
			}
		} 
		if(new_element_count > element_count) {
			element_count = new_element_count;
		}
		index = ELEMENT_COUNT(address->segment_offset, address->type);
		if(IS_SCALAR(container)) {
			container_size = 1;
		} else {
			if(IS_VARIABLE_LENGTH_ARRAY(container)) {
				container_size = 0;
			} else {
				container_size = ARRAY_SIZE(container);
			}
		}
		if(index + element_count > container_size) {
			return FALSE;
		}
	}
	return TRUE;
}

static void ZEND_FASTCALL qb_print_segment_bound_check(qb_native_compiler_context *cxt, qb_address *address, const char *new_size) {
	const char *index = qb_get_segment_index(cxt, address);
	const char *size = (new_size) ? new_size : qb_get_array_size(cxt, address);

	if(size == (char *) "res_count") {
		qb_printf(cxt, "if(UNEXPECTED(res_count > res_count_before || %s + %s > segment_element_count%d || %s + %s < %s)) {\n", index, size, address->segment_selector, index, size, index);
	} else {
		qb_printf(cxt, "if(UNEXPECTED(%s + %s > segment_element_count%d || %s + %s < %s)) {\n", index, size, address->segment_selector, index, size, index);
	}
	qb_printf(cxt, "	qb_abort_range_error(cxt, &cxt->storage->segments[%d], %s, %s, PHP_LINE_NUMBER);\n", address->segment_selector, index, size);
	qb_print(cxt,  "}\n");
}

static void ZEND_FASTCALL qb_print_segment_enlargement(qb_native_compiler_context *cxt, qb_address *address, const char *new_size) {
	const char *index = qb_get_segment_index(cxt, address);
	const char *size = (new_size) ? new_size : qb_get_array_size(cxt, address);

	qb_printf(cxt, "if(%s + %s > segment_element_count%d) {\n", index, size, address->segment_selector);
	qb_printf(cxt, "	qb_enlarge_segment(cxt, &cxt->storage->segments[%d], %s + %s);\n", address->segment_selector, index, size);
	qb_printf(cxt,  "} else if(UNEXPECTED(%s + %s < %s)) {\n", index, size, index);
	qb_printf(cxt, "	qb_abort_range_error(cxt, &cxt->storage->segments[%d], %s, %s, PHP_LINE_NUMBER);\n", address->segment_selector, index, size);
	qb_print(cxt,  "}\n");
}

static void ZEND_FASTCALL qb_print_segment_shrinkage(qb_native_compiler_context *cxt, qb_address *address) {
	const char *index = qb_get_segment_index(cxt, address);
	const char *size = qb_get_array_size(cxt, address);

	qb_printf(cxt, "if(%s + %s <= segment_element_count%d) {\n", index, size, address->segment_selector);
	qb_printf(cxt, "	qb_shrink_segment(cxt, &cxt->storage->segments[%d], %s, %s);\n", address->segment_selector, index, size);
	qb_print(cxt,  "}\n");
}

extern uint32_t global_op_flags[];
extern uint32_t global_operand_flags[];

static zend_always_inline uint32_t qb_get_op_flags(qb_native_compiler_context *cxt, uint32_t opcode) {

	return global_op_flags[opcode];
}

static zend_always_inline uint32_t qb_get_operand_flags(qb_native_compiler_context *cxt, uint32_t opcode, uint32_t operand_index) {
	return (global_operand_flags[opcode] >> (operand_index * 4)) & 0x0F;
}

extern const char compressed_table_op_names[];

static const char * ZEND_FASTCALL qb_get_op_name(qb_native_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->op_names) {
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		cxt->op_names = cxt->pool->op_names;
	}
	return (cxt->op_names) ? cxt->op_names[opcode] : "";
}

static zend_always_inline const char * qb_get_op_action(qb_native_compiler_context *cxt, uint32_t opcode) {
	return cxt->op_actions[opcode];
}

static zend_always_inline const char * qb_get_op_result_size_code(qb_native_compiler_context *cxt, uint32_t opcode) {
	return cxt->op_result_size_codes[opcode];
}

static zend_always_inline const char * qb_get_op_result_size_variables(qb_native_compiler_context *cxt, uint32_t opcode) {
	return cxt->op_result_size_variables[opcode];
}

#define POST_OP_ACTION			1
#define POST_FUNCTION_CALL		2

static void ZEND_FASTCALL qb_print_resynchronization(qb_native_compiler_context *cxt, qb_address *address, int32_t context) {
	if(address->source_address) {
		qb_print_resynchronization(cxt, address->source_address, context);
	} else {
		if(IS_SCALAR(address)) {
			if(!(address->flags & QB_ADDRESS_CONSTANT)) {
				if((address->flags & QB_ADDRESS_AUTO_INCREMENT) || (context == POST_FUNCTION_CALL)) {
					// transfer the value from the segment
					qb_printf(cxt, "%s = %s[0];\n", qb_get_scalar(cxt, address), qb_get_segment_pointer(cxt, address));
				} else if((address->flags & QB_ADDRESS_NON_LOCAL) && (context == POST_OP_ACTION)) {
					// transfer value back to segment
					qb_printf(cxt, "%s[0] = %s;\n", qb_get_segment_pointer(cxt, address), qb_get_scalar(cxt, address));
				}
			}
		} else {
			// make sure the size variable is up-to-date
			qb_print_resynchronization(cxt, address->array_size_address, context);
			if(address->dimension_count > 1) {
				// the dimension also could have changed
				qb_print_resynchronization(cxt, address->dimension_addresses[0], context);
			}
		}
	}
}

static void ZEND_FASTCALL qb_print_op(qb_native_compiler_context *cxt, qb_op *qop, uint32_t qop_index) {
	if(qop->flags & QB_OP_JUMP_TARGET) {
		qb_printf(cxt, "L%04d:\n", qop_index);
	}
	if(qop->opcode != QB_NOP) {
		const char *name;
		qb_operand *operand;

		cxt->current_op = qop;
		if(cxt->print_source) {
			name = qb_get_op_name(cxt, qop->opcode);
			qb_printf(cxt, "// %s (line #%d)\n", name, qop->line_number);
		}

		if(qop->flags & QB_OP_VARIABLE_LENGTH) {
			if(qop->opcode == QB_FCALL_VAR || qop->opcode == QB_FCALL_MIX) {
				uint32_t argument_count = qop->operands[1].argument_count;
				uint32_t symbol_index = qop->operands[2].symbol_index;
				qb_operand *operands = &qop->operands[3];
				zend_function *zfunc = cxt->external_symbols[symbol_index].pointer;
				uint32_t i, k;

				qb_printf(cxt, "#define PHP_LINE_NUMBER	%d\n\n", qop->line_number);
				qb_printf(cxt, "function = cxt->function->external_symbols[%d]->pointer;\n", symbol_index);
				qb_printf(cxt, "qb_initialize_function_call(cxt, function, %d, PHP_LINE_NUMBER);\n", argument_count);

				for(i = 0; i < argument_count; i++) {
					qb_address *address = operands[i].address;
					if(address->dimension_count > 0) {
						qb_printf(cxt, "cxt->array_address.type = %d;\n", address->type);
						qb_printf(cxt, "cxt->array_address.flags = 0x%X;\n", address->flags);
						qb_printf(cxt, "cxt->array_address.segment_selector = %d;\n", address->segment_selector);
						qb_printf(cxt, "cxt->array_address.segment_offset = %s;\n", qb_get_segment_offset(cxt, address));
						qb_printf(cxt, "cxt->array_address.dimension_count = %d;\n", address->dimension_count);
						for(k = 0; k < address->dimension_count; k++) {
							qb_address *dimension_address = address->dimension_addresses[k];
							qb_address *size_address = address->array_size_addresses[k];
							qb_printf(cxt, "cxt->dimension_addresses[%d].flags = 0x%X;\n", k, dimension_address->flags);
							qb_printf(cxt, "cxt->dimension_addresses[%d].segment_offset = %d;\n", k, dimension_address->segment_offset);
							qb_printf(cxt, "cxt->array_size_addresses[%d].flags = 0x%X;\n", k, size_address->flags);
							qb_printf(cxt, "cxt->array_size_addresses[%d].segment_offset = %d;\n", k, size_address->segment_offset);
						}
						qb_print(cxt, "cxt->argument_address = &cxt->array_address;\n");
					} else {
						qb_printf(cxt, "cxt->value_address.type = %d;\n", address->type);
						qb_printf(cxt, "cxt->value_address.segment_selector = %d;\n", address->segment_selector);
						qb_printf(cxt, "cxt->value_address.segment_offset = %s;\n", qb_get_segment_offset(cxt, address));
						qb_print(cxt, "cxt->argument_address = &cxt->value_address;\n");
					}
					qb_printf(cxt, "qb_copy_argument(cxt, %d);\n", i);
				}
				qb_print(cxt, "qb_execute_function_call(cxt);\n");
				for(i = 0; i < argument_count + 1; i++) {
					qb_address *address = operands[i].address;
					zend_arg_info *zarg = (i < zfunc->common.num_args && zfunc->common.arg_info) ? &zfunc->common.arg_info[i] : NULL;

					if(!zarg || zarg->pass_by_reference) {
						if(address->dimension_count > 0) {
							qb_printf(cxt, "cxt->array_address.type = %d;\n", address->type);
							qb_printf(cxt, "cxt->array_address.flags = 0x%X;\n", address->flags);
							qb_printf(cxt, "cxt->array_address.segment_selector = %d;\n", address->segment_selector);
							qb_printf(cxt, "cxt->array_address.segment_offset = %s;\n", qb_get_segment_index(cxt, address));
							qb_printf(cxt, "cxt->array_address.dimension_count = %d;\n", address->dimension_count);
							for(k = 0; k < address->dimension_count; k++) {
								qb_address *dimension_address = address->dimension_addresses[k];
								qb_address *size_address = address->array_size_addresses[k];
								qb_printf(cxt, "cxt->dimension_addresses[%d].flags = 0x%X;\n", k, dimension_address->flags);
								qb_printf(cxt, "cxt->dimension_addresses[%d].segment_offset = %d;\n", k, dimension_address->segment_offset);
								qb_printf(cxt, "cxt->array_size_addresses[%d].flags = 0x%X;\n", k, size_address->flags);
								qb_printf(cxt, "cxt->array_size_addresses[%d].segment_offset = %d;\n", k, size_address->segment_offset);
							}
							qb_print(cxt, "cxt->argument_address = &cxt->array_address;\n");
						} else {
							qb_printf(cxt, "cxt->value_address.type = %d;\n", address->type);
							qb_printf(cxt, "cxt->value_address.segment_selector = %d;\n", address->segment_selector);
							qb_printf(cxt, "cxt->value_address.segment_offset = %s;\n", qb_get_segment_offset(cxt, address));
							qb_print(cxt, "cxt->argument_address = &cxt->value_address;\n");
						}
						qb_printf(cxt, "qb_resync_argument(cxt, %d);\n", i);
						qb_print_resynchronization(cxt, address, POST_FUNCTION_CALL);
					}
				}
				qb_print(cxt, "qb_finalize_function_call(cxt);\n");
				qb_print(cxt, "if(cxt->exception_encountered) {\n");
				qb_print(cxt, "	goto L_EXIT;\n");
				qb_print(cxt, "}\n");

				// resync class, instance, global, and static variables
				for(i = 0; i < cxt->variable_count; i++) {
					qb_variable *qvar = cxt->variables[i];
					if(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_GLOBAL | QB_VARIABLE_STATIC)) {
						qb_print_resynchronization(cxt, qvar->address, POST_FUNCTION_CALL);
					}
				}

#if ZEND_DEBUG
				qb_printf(cxt, "#undef PHP_LINE_NUMBER\n");
#endif
			}
		} else {
			const char *action, *jump_targets[2];
			qb_address *result_address = NULL, *container;
			uint32_t operand_number, operand_flags, operand_type, operand_address_mode;
			uint32_t i, jump_target_count = 0, backward_jump_count = 0;

			// define the operands
			operand_number = 1;
			if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
				qb_printf(cxt, "#define PHP_LINE_NUMBER	%d\n", qop->line_number);
			}
			if(qop->flags & QB_OP_NEED_MATRIX_DIMENSIONS) {
				qb_printf(cxt, "#define MATRIX1_ROWS	%d\n", qop->matrix_dimensions >> 20);
				qb_printf(cxt, "#define MATRIX1_COLS	%d\n", (qop->matrix_dimensions >> 10) & 0x03FF);
				qb_printf(cxt, "#define MATRIX2_ROWS	MATRIX1_COLS\n");
				qb_printf(cxt, "#define MATRIX2_COLS	%d\n", qop->matrix_dimensions & 0x03FF);
			}
			for(i = 0; i < qop->operand_count; i++) {
				operand = &qop->operands[i];
				operand_flags = qb_get_operand_flags(cxt, qop->opcode, i);
				if(operand->type >= QB_OPERAND_ADDRESS_VAR && operand->type <= QB_OPERAND_ADDRESS_ARR) {
					qb_address *address = operand->address;
					operand_type = operand_flags & 0x07;
					operand_address_mode = operand_type - 1;
					if(operand_address_mode == QB_ADDRESS_MODE_ARR) {
						const char *pointer = qb_get_pointer(cxt, address);
						const char *size = qb_get_array_size(cxt, address);
						if(operand_flags & QB_OPERAND_WRITABLE) {
							qb_printf(cxt, "#define res_start	%s\n", pointer);
							qb_printf(cxt, "#define res_end	(res_start + res_count)\n");
							qb_printf(cxt, "#define res_ptr	op%d_ptr_%s\n", operand_number, type_names[address->type]);
							qb_printf(cxt, "#define res	res_ptr[0]\n");
						} else {
							qb_printf(cxt, "#define op%d_start	%s\n", operand_number, pointer);
							qb_printf(cxt, "#define op%d_end	(op%d_start + %s)\n", operand_number, operand_number, size);
							qb_printf(cxt, "#define op%d_ptr	op%d_ptr_%s\n", operand_number, operand_number, type_names[address->type]);
							qb_printf(cxt, "#define op%d_count	%s\n", operand_number, size);
							qb_printf(cxt, "#define op%d	op%d_ptr[0]\n", operand_number, operand_number);
						}
					} else {
						const char *scalar = qb_get_scalar(cxt, address);
						if(operand_flags & QB_OPERAND_WRITABLE) {
							qb_printf(cxt, "#define res	%s\n", scalar);
						} else {
							qb_printf(cxt, "#define op%d	%s\n", operand_number, scalar);
							if(qop->flags & QB_OP_ISSET && operand_address_mode == QB_ADDRESS_MODE_ELV) {
								qb_printf(cxt, "#define op%d_ptr	((%s *) isset_pointer)\n", operand_number, type_cnames[address->type]);
							}
						}
					}
					operand_number++;
				} else if(operand->type == QB_OPERAND_JUMP_TARGET) {
					jump_targets[jump_target_count++] = qb_get_jump(cxt, operand->jump_target_index, qop_index);
					if(qb_get_jump_direction(cxt, operand->jump_target_index, qop_index) < 0) {
						backward_jump_count++;
					}
				}
			}
			qb_print(cxt, "\n");

			// perform bound checking
			operand_number = 1;
			for(i = 0; i < qop->operand_count; i++) {
				operand = &qop->operands[i];
				operand_flags = qb_get_operand_flags(cxt, qop->opcode, i);
				if(operand->type >= QB_OPERAND_ADDRESS_VAR && operand->type <= QB_OPERAND_ADDRESS_ARR) {
					qb_address *address = operand->address;
					container = qb_get_root_container(cxt, address);
					operand_type = operand_flags & 0x07;
					operand_address_mode = operand_type - 1;
					if(operand_flags & QB_OPERAND_WRITABLE) {

						if(operand_address_mode == QB_ADDRESS_MODE_ARR) {
							if(!(qop->flags & QB_OP_UNSET)) {
								const char *result_size_code, *result_size_variables, *result_size;

								// save the current size to a variable first
								result_size = qb_get_array_size(cxt, address);
								qb_printf(cxt, "res_count = res_count_before = %s;\n", result_size);

								// check a list of variables too see if they're bigger
								result_size_variables = qb_get_op_result_size_variables(cxt, qop->opcode);
								if(result_size_variables) {
									const char *var;
									uint32_t var_len, new_element_count;
									int32_t need_expansion_check = FALSE;

									// run any code needed to find out how large the result ought to be
									result_size_code = qb_get_op_result_size_code(cxt, qop->opcode);
									if(result_size_code) {
										qb_print(cxt, result_size_code);
									}

									for(var = result_size_variables; var[0] != '\0'; var += var_len) {
										uint32_t number;
										var_len = strlen(var);

										if(var_len == 9 && sscanf(var, "op%d_count", &number) == 1) {
											// the result should be at least the size of this input operand
											qb_operand *source_operand = &qop->operands[i - (operand_number - number)];
											if(IS_FIXED_LENGTH_ARRAY(source_operand->address)) {
												new_element_count = ARRAY_SIZE(source_operand->address);
											} else if(IS_EXPANDABLE_ARRAY(source_operand->address)) {
												new_element_count = UINT32_MAX;
											} else {
												new_element_count = 1;
											}
										} else if(sscanf(var, "%d", &number) == 1) {
											// the result should be at least this fixed number
											new_element_count = number;
										} else {
											// can't tell at compile time
											new_element_count = UINT32_MAX;
										}

										if(!qb_is_always_in_bound(cxt, container, address, new_element_count)) {
											qb_printf(cxt, "if(%s > res_count) {\n", var);
											qb_printf(cxt, "	res_count = %s;\n", var);
											qb_printf(cxt, "}\n");
											need_expansion_check = TRUE;
										}
									}

									if(need_expansion_check) {
										if(IS_EXPANDABLE_ARRAY(container)) {
											qb_print_segment_enlargement(cxt, address, "res_count");
										} else {
											qb_print_segment_bound_check(cxt, address, "res_count");
										}
									}
								}
								qb_printf(cxt, "res_ptr = res_start;\n");
							} else {
								// unset the array
								qb_print_segment_shrinkage(cxt, address);
							}
						} else if(operand_address_mode == QB_ADDRESS_MODE_ELC || operand_address_mode == QB_ADDRESS_MODE_ELV) {
							if(!(qop->flags & QB_OP_UNSET)) {
								if(!qb_is_always_in_bound(cxt, container, address, 0)) {
									if(IS_EXPANDABLE_ARRAY(container)) {
										qb_print_segment_enlargement(cxt, address, NULL);
									} else {
										qb_print_segment_bound_check(cxt, address, NULL);
									}
								}
							} else {
								// move elements behind this one forward
								qb_print_segment_shrinkage(cxt, address);
							}
						}
						result_address = address;
					} else {
						if(address->mode == QB_ADDRESS_MODE_ARR) {
							if(!qb_is_always_in_bound(cxt, container, address, 0)) {
								qb_print_segment_bound_check(cxt, address, NULL);
							}
						} else if(address->mode == QB_ADDRESS_MODE_ELC || address->mode == QB_ADDRESS_MODE_ELV) {
							if(!(qop->flags & QB_OP_ISSET)) {
								if(!qb_is_always_in_bound(cxt, container, address, 0)) {
									qb_print_segment_bound_check(cxt, address, NULL);
								}
							} else {
								const char *index = qb_get_segment_index(cxt, address);
								const char *pointer = qb_get_segment_pointer(cxt, address);

								qb_printf(cxt, "if(%s < segment_element_count%d) {\n", index, address->segment_selector);
								qb_printf(cxt, 		"isset_pointer = %s;\n", pointer);
								qb_print(cxt,  "} else {\n");
								qb_print(cxt,		"isset_pointer = NULL;\n");
								qb_print(cxt,  "}\n");
							}
						}
						if(operand_address_mode == QB_ADDRESS_MODE_ARR) {
							qb_printf(cxt, "op%d_ptr = op%d_start;\n", operand_number, operand_number);
						}
					}
					operand_number++;
				}
			}

			// print code that actually performs the action
			action = qb_get_op_action(cxt, qop->opcode);
			if(action) {
				qb_print(cxt, action);
				qb_print(cxt, "\n");
			}

			// resync scalars
			if(result_address) {
				qb_print_resynchronization(cxt, result_address, POST_OP_ACTION);
			}
			qb_print(cxt, "\n");


			// perform jump
			if(qop->flags & QB_OP_JUMP) {
#ifdef ZEND_WIN32				
				// check for timed-out condition on any backward jump
				if(backward_jump_count > 0) {
					qb_print(cxt, "if(*cxt->windows_timed_out_pointer) {\n");
					qb_print(cxt,		"zend_timeout(1);\n");
					qb_print(cxt,		"goto L_EXIT;\n");
					qb_print(cxt, "}\n");
				}
#endif
				if(jump_target_count == 2) {
					if(jump_targets[1]) {
						if(jump_targets[0]) {
							qb_printf(cxt, "if(condition) %s; else %s;\n", jump_targets[0], jump_targets[1]);
						} else {
							qb_printf(cxt, "if(!condition) %s;\n", jump_targets[1]);
						}
					} else {
						qb_printf(cxt, "if(condition) %s;\n", jump_targets[0]);
					}
				} else if(jump_target_count == 1) {
					if(jump_targets[0]) {
						qb_printf(cxt, "%s;\n", jump_targets[0]);
					}
				} else {
					qb_print(cxt, "goto L_EXIT;\n");
				}
				qb_print(cxt, "\n");
			}

#if ZEND_DEBUG
			operand_number = 1;
			for(i = 0; i < qop->operand_count; i++) {
				operand = &qop->operands[i];
				operand_flags = qb_get_operand_flags(cxt, qop->opcode, i);
				if(operand->type >= QB_OPERAND_ADDRESS_VAR && operand->type <= QB_OPERAND_ADDRESS_ARR) {
					operand_type = operand_flags & 0x07;
					operand_address_mode = operand_type - 1;
					if(operand_address_mode == QB_ADDRESS_MODE_ARR) {
						if(operand_flags & QB_OPERAND_WRITABLE) {
							qb_printf(cxt, "#undef res_start\n");
							qb_printf(cxt, "#undef res_end\n");
							qb_printf(cxt, "#undef res_ptr\n");
							qb_printf(cxt, "#undef res\n");
						} else {
							qb_printf(cxt, "#undef op%d_start\n", operand_number);
							qb_printf(cxt, "#undef op%d_end\n", operand_number);
							qb_printf(cxt, "#undef op%d_ptr\n", operand_number);
							qb_printf(cxt, "#undef op%d_count\n", operand_number);
							qb_printf(cxt, "#undef op%d\n", operand_number);
						}
					} else {
						if(operand_flags & QB_OPERAND_WRITABLE) {
							qb_printf(cxt, "#undef res\n");
						} else {
							qb_printf(cxt, "#undef op%d\n", operand_number);
							if(qop->flags & QB_OP_ISSET && operand_address_mode == QB_ADDRESS_MODE_ELV) {
								qb_printf(cxt, "#undef op%d_ptr\n", operand_number);
							}
						}
					}
					operand_number++;
				}
			}
			if(qop->flags & QB_OP_NEED_MATRIX_DIMENSIONS) {
				qb_print(cxt, "#undef MATRIX1_ROWS\n");
				qb_print(cxt, "#undef MATRIX1_COLS\n");
				qb_print(cxt, "#undef MATRIX2_ROWS\n");
				qb_print(cxt, "#undef MATRIX2_COLS\n");
			}
			if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
				qb_print(cxt, "#undef PHP_LINE_NUMBER\n");
			}
			qb_print(cxt, "\n");
#endif
		}
	}
}

static void ZEND_FASTCALL qb_print_local_variables(qb_native_compiler_context *cxt) {
	uint32_t i, j, need_pointer[QB_TYPE_COUNT][6];

	// print local variables, declarations first
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "int8_t *restrict segment%d = cxt->storage->segments[%d].memory;\n", i, i);
	}
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "uint32_t segment_element_count%d = *cxt->storage->segments[%d].array_size_pointer;\n", i, i);
	}
	qb_print(cxt, "\n");

	// write variables that can be local
	for(i = 0; i < cxt->scalar_count; i++) {
		qb_address *scalar = cxt->scalars[i];
		if(!(scalar->flags & QB_ADDRESS_CONSTANT)) {
			qb_printf(cxt, "%s %s = %s[0];\n", type_cnames[scalar->type], qb_get_scalar(cxt, scalar), qb_get_segment_pointer(cxt, scalar));
		}
	}

	qb_print(cxt, "\n");
	qb_print(cxt, "uint32_t vector_count, matrix1_count, matrix2_count, mmult_res_count;\n");
	qb_print(cxt, "uint32_t string_length, symbol_index;\n");
	qb_print(cxt, "uint32_t condition, res_count, res_count_before;\n");
	qb_print(cxt, "void * isset_pointer;\n");
	qb_print(cxt, "zend_function *function;\n");
	qb_print(cxt, "\n");

	// see which loop pointers are needed
	memset(need_pointer, 0, sizeof(need_pointer));
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		uint32_t operand_flags, operand_address_mode, operand_index = 0;
		for(j = 0; (operand_flags = qb_get_operand_flags(cxt, qop->opcode, j)); j++) {
			operand_address_mode = operand_flags & 0x07;
			if(QB_OPERAND_ADDRESS_VAR <= operand_address_mode && operand_address_mode <= QB_OPERAND_ADDRESS_ARR) {
				if(operand_address_mode == QB_OPERAND_ADDRESS_ARR) {
					qb_address *address = qop->operands[j].address;
					need_pointer[address->type][operand_index] = TRUE;
				}
				operand_index++;
			}
		}
	}

	// print the loop pointers
	for(i = 0; i < QB_TYPE_COUNT; i++) {
		uint32_t var_printed = 0;
		for(j = 0; j < 6; j++) {
			if(need_pointer[i][j]) {
				const char *type = type_names[i];
				if(var_printed == 0) {
					const char *ctype = type_cnames[i];
					qb_printf(cxt, "%s *op%d_ptr_%s", ctype, j + 1, type);
				} else {
					qb_printf(cxt, ", *op%d_ptr_%s", j + 1, type);
				}
				var_printed++;
			}
		}
		if(var_printed) {
			qb_printf(cxt, ";\n");
		}
	}
	qb_print(cxt, "\n");
#ifdef ZTS
		qb_print(cxt, STRING(USE_TSRM)"\n");
#endif
	qb_print(cxt, "\n");
	// sanity check
	qb_printf(cxt, "if(cxt->function->instruction_crc64 != 0x%" PRIX64 "ULL) {\n", cxt->instruction_crc64);
	qb_print( cxt, "	return " STRING(FAILURE) ";\n");
	qb_print( cxt, "}\n");

	// set the stack variable references
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "cxt->storage->segments[%d].stack_ref_memory = &segment%d;\n", i, i);
	}
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "cxt->storage->segments[%d].stack_ref_element_count = &segment_element_count%d;\n", i, i);
	}
	qb_print(cxt, "\n");
}

static void ZEND_FASTCALL qb_print_exit_section(qb_native_compiler_context *cxt) {
	uint32_t i;

	qb_print(cxt, "L_EXIT:\n");
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "cxt->storage->segments[%d].stack_ref_memory = &cxt->storage->segments[%d].memory;\n", i, i);
	}
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_printf(cxt, "cxt->storage->segments[%d].stack_ref_element_count = &cxt->storage->segments[%d].element_count;\n", i, i);
	}
	qb_print(cxt, "return " STRING(SUCCESS) ";\n");
}

static void ZEND_FASTCALL qb_print_function(qb_native_compiler_context *cxt) {
	uint32_t i;
	if(cxt->print_source) {
		qb_printf(cxt, "\n// Handle for %s()\n", cxt->function_name);
	}
	qb_printf(cxt, STRING(QB_NATIVE_FUNCTION_RET QB_NATIVE_FUNCTION_ATTR) " QBN_%" PRIX64 "(" STRING(QB_NATIVE_FUNCTION_ARGS) ")\n{\n", cxt->instruction_crc64);
	qb_print_local_variables(cxt);
	for(i = 0; i < cxt->op_count; i++) {
		qb_print_op(cxt, cxt->ops[i], i);
	}
	qb_print_exit_section(cxt);
	qb_print(cxt, "}\n");
}

static void ZEND_FASTCALL qb_print_version(qb_native_compiler_context *cxt) {
	qb_printf(cxt, "\nuint32_t QB_VERSION = 0x%08x;\n\n", QB_VERSION_SIGNATURE);
}

static void ZEND_FASTCALL qb_print_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		if(!compiler_cxt->native_proc && (compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE)) {
			// check if an earlier function has the same crc64
			int32_t duplicate = FALSE;
			for(j = 0; j < i; j++) {
				if(cxt->compiler_contexts[j].instruction_crc64 == compiler_cxt->instruction_crc64) {
					duplicate = TRUE;
					break;
				}
			}
			if(!duplicate) {
				// copy variables from the compiler context to make life easier
				cxt->ops = compiler_cxt->ops;
				cxt->op_count = compiler_cxt->op_count;
				cxt->op_translations = compiler_cxt->op_translations;
				cxt->variables = compiler_cxt->variables;
				cxt->variable_count = compiler_cxt->variable_count;
				cxt->scalars = compiler_cxt->scalars;
				cxt->scalar_count = compiler_cxt->scalar_count;
				cxt->arrays = compiler_cxt->arrays;
				cxt->array_count = compiler_cxt->array_count;
				cxt->external_symbols = compiler_cxt->external_symbols;
				cxt->external_symbol_count = compiler_cxt->external_symbol_count;
				cxt->storage = compiler_cxt->storage;
				cxt->instruction_crc64 = compiler_cxt->instruction_crc64;
				cxt->function_name = compiler_cxt->zend_function->common.function_name;

				// print the function
				qb_print_function(cxt);
			}
		}
	}
}

static void ZEND_FASTCALL qb_print_function_records(qb_native_compiler_context *cxt) {
	uint32_t i;
	qb_print(cxt, "\n");
	qb_print(cxt, "#define HAVE_NATIVE_PROC_RECORDS\n");
	qb_print(cxt, "qb_native_proc_record native_proc_records[] = {\n");
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		if(!compiler_cxt->native_proc && (compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE)) {
			qb_printf(cxt, "	{ 0x%" PRIX64 "ULL, QBN_%" PRIX64 " },\n", compiler_cxt->instruction_crc64, compiler_cxt->instruction_crc64);
		}
	}
	qb_print(cxt, "};\n");
}

static void * ZEND_FASTCALL qb_find_symbol(qb_native_compiler_context *cxt, const char *name) {
	long hash_value;
	uint32_t i, name_len = strlen(name);
#if defined(_MSC_VER)
	char name_buffer[256];
	if(name_len > sizeof(name_buffer) - 1) {
		return NULL;
	}
	if(name[0] == '_') {
		name_len--;
		strncpy(name_buffer, name + 1, name_len);
	} else if(name[0] == '@') {
		// '@' means fastcall
		while(name[--name_len] != '@');
		name_len--;
		strncpy(name_buffer, name + 1, name_len);
	} else {
		strncpy(name_buffer, name, name_len);
	}
	name_buffer[name_len] = '\0';
	name = name_buffer;
#elif defined(__MACH__)
	name++;
	name_len--;
#endif
	hash_value = zend_get_hash_value(name, name_len + 1);
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				return symbol->address;
			}
		}
	}
	return NULL;
}

#ifdef __GNUC__
static int32_t ZEND_FASTCALL qb_wait_for_compiler_response(qb_native_compiler_context *cxt) {
	// close the write stream
	fclose(cxt->write_stream);
	cxt->write_stream = NULL;

	// read output from stderr
	char buffer[256];
	int count;
	while((count = fread(buffer, 1, sizeof(buffer), cxt->error_stream))) {
		if(cxt->print_errors) {
			php_write(buffer, count);
		}
	}

	// wait for the gcc to finish
	int status;
	wait(&status);

	if(status == -1) {
		return FALSE;
	}
	return TRUE;
}
#endif

#ifdef _MSC_VER
static int32_t ZEND_FASTCALL qb_wait_for_compiler_response(qb_native_compiler_context *cxt) {
	USE_TSRM
	char buffer[256];
	int count;
	DWORD time_limit = EG(timeout_seconds) ? EG(timeout_seconds) * 1000000 : INFINITE;

	// close the write stream
	fclose(cxt->write_stream);
	cxt->write_stream = NULL;

	// the compiler process was created in suspended state
	// resume it and wait for it to finish
	ResumeThread(cxt->msc_thread);

	// read output from stderr
	while((count = fread(buffer, 1, sizeof(buffer), cxt->error_stream))) {
		if(cxt->print_errors) {
			php_write(buffer, count TSRMLS_CC);
		}
	}

	if(WaitForSingleObject(cxt->msc_process, time_limit) == WAIT_TIMEOUT) {
		TerminateProcess(cxt->msc_process, -1);
		return FALSE;
	}

	// delete the temporary c file
	DeleteFile(cxt->c_file_path);
	return TRUE;
}
#endif	// _MSC_VER

#ifdef __GNUC__
static int32_t ZEND_FASTCALL qb_load_object_file(qb_native_compiler_context *cxt) {
	// map the file into memory 
	int file_descriptor = open(cxt->obj_file_path, O_RDWR);
	size_t binary_size;
	char *binary = NULL;
	if(file_descriptor != -1) {
		struct stat stat_buf;
		if(fstat(file_descriptor, &stat_buf) != -1) {
			binary_size = stat_buf.st_size;
#ifdef __LP64__
			// on x64-64, gcc is going to use 32-bit relative pointers for function calls
			// the code has to be thus located in an address not too far from the PHP executable
			// assume here that it's located with in the first 4 gig
			binary = mmap(NULL, binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_32BIT, file_descriptor, 0);
#else
			binary = mmap(NULL, binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, file_descriptor, 0);
#endif
		}
		close(file_descriptor);
	}
	if(!binary) {
		return FALSE;
	}
	cxt->binary = binary;
	cxt->binary_size = binary_size;
	return TRUE;
}

static void ZEND_FASTCALL qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		munmap(cxt->binary, cxt->binary_size);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	unlink(cxt->obj_file_path);
}

#endif	// __GNUC__

#ifdef _MSC_VER
static int32_t ZEND_FASTCALL qb_load_object_file(qb_native_compiler_context *cxt) {
	USE_TSRM
	HANDLE file = NULL, mapping = NULL;

	file = CreateFile(cxt->obj_file_path, GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if(file != INVALID_HANDLE_VALUE) {
		mapping = CreateFileMapping(file, NULL, PAGE_EXECUTE_WRITECOPY, 0, 0, NULL);
		if(mapping != INVALID_HANDLE_VALUE) {
			cxt->binary = MapViewOfFileEx(mapping, FILE_MAP_EXECUTE | FILE_MAP_COPY, 0, 0, 0, NULL);
			cxt->binary_size = GetFileSize(file, NULL);
		}
	}

	CloseHandle(mapping);
	CloseHandle(file);		
	return (cxt->binary != NULL);
}

static void ZEND_FASTCALL qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		UnmapViewOfFile(cxt->binary);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	DeleteFile(cxt->obj_file_path);
}
#endif

#ifdef __GNUC__
static void ZEND_FASTCALL qb_lock_object_file(qb_native_compiler_context *cxt) {
	mprotect(cxt->binary, cxt->binary_size, PROT_EXEC | PROT_READ);
}
#endif

#ifdef _MSC_VER
static void ZEND_FASTCALL qb_lock_object_file(qb_native_compiler_context *cxt) {
	VirtualProtect(cxt->binary, cxt->binary_size, FILE_MAP_EXECUTE | FILE_MAP_READ, NULL);
}
#endif

static uint32_t ZEND_FASTCALL qb_attach_symbol(qb_native_compiler_context *cxt, const char *symbol_name, const char *address) {
	uint32_t count = 0;
	if(strncmp(symbol_name, "QBN_", 4) == 0) {
		qb_native_proc proc = (qb_native_proc) address;
		uint64_t crc64 = strtoull(symbol_name + 4, NULL, 16);
		uint32_t i;
		for(i = 0; i < cxt->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
			if(compiler_cxt->instruction_crc64 == crc64) {
				compiler_cxt->native_proc = proc;
				count++;
			}
		}
	} 
	return count;
}

static void ZEND_FASTCALL qb_detach_symbols(qb_native_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		compiler_cxt->native_proc = NULL;
	}
	cxt->qb_version = 0;
}

#ifdef __ELF__
	#ifdef __LP64__
static int32_t ZEND_FASTCALL qb_parse_elf64(qb_native_compiler_context *cxt) {
	if(cxt->binary_size < sizeof(Elf64_Ehdr)) {
		return FALSE;
	}

	// see if it's has the ELF signature (the fifth byte indicates whether it's 32 or 64 bit)
	Elf64_Ehdr *header = (Elf64_Ehdr *) cxt->binary;
	if(memcmp(header->e_ident, "\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) != 0) {
		return FALSE;
	}
		#ifdef __x86_64__
	if(header->e_machine != EM_X86_64) {
		return FALSE;
	}
		#endif

	// check the size
	int section_header_end = header->e_shoff + header->e_shentsize * header->e_shnum;
	if(section_header_end > cxt->binary_size) {
		return FALSE;
	}

	int i;
	int section_count = header->e_shnum;
	Elf64_Shdr *section_headers = (Elf64_Shdr *) (cxt->binary + header->e_shoff);
	Elf64_Shdr *section_string_section_header = &section_headers[header->e_shstrndx];
	char *section_string_section = (char *) (cxt->binary + section_string_section_header->sh_offset);
	char *string_section = NULL;
	char *text_section = NULL;
	Elf64_Rela *relocations = NULL;
	int relocation_count = 0;
	Elf64_Sym *symbols = NULL;
	int symbol_count = 0;

	// look up the relocation table, symbol table, string table, and text section,
	for(i = 0; i < section_count; i++) {
		Elf64_Shdr *section_header = &section_headers[i];
		char *section_name = section_string_section + section_header->sh_name;
		int section_size = section_header->sh_size;

		if(section_header->sh_type == SHT_RELA && memcmp(section_name, ".rela.text", 11) == 0) {
			relocations = (Elf64_Rela *) (cxt->binary + section_header->sh_offset);
			relocation_count = section_size / sizeof(Elf64_Rela);
		} else if(section_header->sh_type == SHT_PROGBITS && memcmp(section_name, ".text", 6) == 0) {
			text_section = (char *) (cxt->binary + section_header->sh_offset);
		} else if(section_header->sh_type == SHT_SYMTAB && memcmp(section_name, ".symtab", 8) == 0) {
			symbols = (Elf64_Sym *) (cxt->binary + section_header->sh_offset);
			symbol_count = section_size / sizeof(Elf64_Sym);
		} else if(section_header->sh_type == SHT_STRTAB && memcmp(section_name, ".strtab", 8) == 0) {
			string_section = (char *) (cxt->binary + section_header->sh_offset);
		}
	}

	// perform relocations
	for(i = 0; i < relocation_count; i++) {
		Elf64_Rela *relocation = &relocations[i];
		int reloc_type = ELF64_R_TYPE(relocation->r_info);
		int symbol_index = ELF64_R_SYM(relocation->r_info);
		Elf64_Sym *symbol = &symbols[symbol_index];
		int symbol_bind = ELF64_ST_BIND(symbol->st_info);
		char *symbol_name = string_section + symbol->st_name;
		void *symbol_address;
		void *target_address = (text_section + relocation->r_offset);

		if(symbol_bind == STB_LOCAL) {
			symbol_address = (cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value);
		} else if(symbol_bind == STB_GLOBAL) {
			symbol_address = qb_find_symbol(cxt, symbol_name);
			if(!symbol_address) {
				qb_abort("Missing symbol: %s\n", symbol_name);
			}
		} else {
			return FALSE;
		}

		#ifdef __x86_64__
		int64_t A = relocation->r_addend;
		int64_t P = (uint64_t) target_address;
		int64_t S = (uint64_t) symbol_address;

		switch(reloc_type) {
			case R_X86_64_NONE:
				break;
			case R_X86_64_64:
				*((int64_t *) target_address) = S + A;
				break;
			case R_X86_64_PC32:
				*((uint32_t *) target_address) = S + A - P;
				break;
			case R_X86_64_32:
				*((uint32_t *) target_address) = S + A;
				break;
			case R_X86_64_32S:
				*((int32_t *) target_address) = S + A;
				break;
			default:
				return FALSE;
		}
		#endif
	}

	// find the compiled functions
	uint32_t count = 0;
	for(i = 0; i < symbol_count; i++) {
		Elf64_Sym *symbol = &symbols[i];
		if(symbol->st_shndx < symbol_count) {
			int symbol_type = ELF64_ST_TYPE(symbol->st_info);
			char *symbol_name = string_section + symbol->st_name;
			void *symbol_address = cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value;
			if(symbol_type == STT_FUNC) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name, symbol_address);
				if(!attached) {
					// error out if there's an unrecognized function
					if(!qb_find_symbol(cxt, symbol_name)) {
						return FALSE;
					}
				}
				count += attached;
			} else if(symbol_type == STT_OBJECT) {
				if(strncmp(symbol_name, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}
	#else
static int32_t ZEND_FASTCALL qb_parse_elf32(qb_native_compiler_context *cxt) {
	if(cxt->binary_size < sizeof(Elf32_Ehdr)) {
		return FALSE;
	}

	// see if it's has the ELF signature
	Elf32_Ehdr *header = (Elf32_Ehdr *) cxt->binary;
	if(memcmp(header->e_ident, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) != 0) {
		return FALSE;
	}
		#ifdef __i386__
	if(header->e_machine != EM_386) {
		return FALSE;
	}
		#endif

	// check the size
	int section_header_end = header->e_shoff + header->e_shentsize * header->e_shnum;
	if(section_header_end > cxt->binary_size) {
		return FALSE;
	}

	int i;
	int section_count = header->e_shnum;
	Elf32_Shdr *section_headers = (Elf32_Shdr *) (cxt->binary + header->e_shoff);
	Elf32_Shdr *section_string_section_header = &section_headers[header->e_shstrndx];
	char *section_string_section = (char *) (cxt->binary + section_string_section_header->sh_offset);
	char *string_section = NULL;
	char *text_section = NULL;
	Elf32_Rel *relocations = NULL;
	int relocation_count = 0;
	Elf32_Sym *symbols = NULL;
	int symbol_count = 0;

	// look up the relocation table, symbol table, string table, and text section,
	for(i = 0; i < section_count; i++) {
		Elf32_Shdr *section_header = &section_headers[i];
		char *section_name = section_string_section + section_header->sh_name;
		int section_size = section_header->sh_size;

		if(section_header->sh_type == SHT_REL && memcmp(section_name, ".rel.text", 10) == 0) {
			relocations = (Elf32_Rel *) (cxt->binary + section_header->sh_offset);
			relocation_count = section_size / sizeof(Elf32_Rel);
		} else if(section_header->sh_type == SHT_PROGBITS && memcmp(section_name, ".text", 6) == 0) {
			text_section = (char *) (cxt->binary + section_header->sh_offset);
		} else if(section_header->sh_type == SHT_SYMTAB && memcmp(section_name, ".symtab", 8) == 0) {
			symbols = (Elf32_Sym *) (cxt->binary + section_header->sh_offset);
			symbol_count = section_size / sizeof(Elf32_Sym);
		} else if(section_header->sh_type == SHT_STRTAB && memcmp(section_name, ".strtab", 8) == 0) {
			string_section = (char *) (cxt->binary + section_header->sh_offset);
		}
	}

	// perform relocations
	for(i = 0; i < relocation_count; i++) {
		Elf32_Rel *relocation = &relocations[i];
		int reloc_type = ELF32_R_TYPE(relocation->r_info);
		int symbol_index = ELF32_R_SYM(relocation->r_info);
		Elf32_Sym *symbol = &symbols[symbol_index];
		int symbol_bind = ELF32_ST_BIND(symbol->st_info);
		char *symbol_name = string_section + symbol->st_name;
		void *symbol_address;
		void *target_address = (text_section + relocation->r_offset);

		if(symbol_bind == STB_LOCAL) {
			symbol_address = (cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value);
		} else if(symbol_bind == STB_GLOBAL) {
			symbol_address = qb_find_symbol(cxt, symbol_name);
			if(!symbol_address) {
				qb_abort("Missing symbol: %s\n", symbol_name);
			}
		} else {
			return FALSE;
		}

		#ifdef __i386__
		int32_t A = *((uint32_t *) target_address);
		int32_t P = (uint32_t) target_address;
		int32_t S = (uint32_t) symbol_address;

		switch(reloc_type) {
			case R_386_NONE:
				break;
			case R_386_32:
				*((uint32_t *) target_address) = S + A;
				break;
			case R_386_PC32:
				*((uint32_t *) target_address) = S + A - P;
				break;
			default:
				return FALSE;
		}
		#endif
	}

	// find the compiled functions
	uint32_t count = 0;
	for(i = 0; i < symbol_count; i++) {
		Elf32_Sym *symbol = &symbols[i];
		if(symbol->st_shndx < symbol_count) {
			int symbol_type = ELF32_ST_TYPE(symbol->st_info);
			char *symbol_name = string_section + symbol->st_name;
			void *symbol_address = cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value;
			if(symbol_type == STT_FUNC) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name, symbol_address);
				if(!attached) {
					// error out if there's an unrecognized function
					if(!qb_find_symbol(cxt, symbol_name)) {
						return FALSE;
					}
				}
				count += attached;
			} else if(symbol_type == STT_OBJECT) {
				if(strncmp(symbol_name, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}
	#endif	// __LP64__
#endif	// __ELF__

#ifdef __MACH__
	#ifdef __LP64__
static int32_t ZEND_FASTCALL qb_parse_macho64(qb_native_compiler_context *cxt) {
	struct mach_header_64 *header;
	struct load_command *command;
	struct segment_command_64 *segment_command;
	struct section_64 *sections = NULL;
	struct symtab_command *symtab_command;
	struct nlist_64 *symbols = NULL;
	char *string_table;
	struct relocation_info *relocations;
	uint32_t relocation_count;
	char *text_section = NULL;
	uint32_t text_section_number = 0;
	uint32_t i;

	if(cxt->binary_size < sizeof(struct mach_header_64)) {
		return FALSE;
	}

	header = (struct mach_header_64 *) cxt->binary;
	if(header->magic != MH_MAGIC_64 || header->filetype != MH_OBJECT) {
		return FALSE;
	}
		#ifdef __x86_64__
	if(header->cputype != CPU_TYPE_X86_64) {
		return FALSE;
	}
		#endif

	command = (struct load_command *) (cxt->binary + sizeof(struct mach_header_64));
	for(i = 0; i < header->ncmds; i++) {
		if(command->cmd == LC_SEGMENT_64) {
			// there's only one segment inside a mach-o object file
			// section structures of the segment are located right after the segment command
			segment_command = (struct segment_command_64 *) command;
			sections = (struct section_64 *) ((char *) command + sizeof(struct segment_command_64));
		} else if(command->cmd == LC_SYMTAB) {
			symtab_command = (struct symtab_command *) command;
			symbols = (struct nlist_64 *) (cxt->binary + symtab_command->symoff);
			string_table = cxt->binary + symtab_command->stroff;
		}
		command = (struct load_command *) (((char *) command) + command->cmdsize);
	}

	if(!sections || !symbols) {
		return FALSE;
	}

	for(i = 0; i < segment_command->nsects; i++) {
		struct section_64 *section = &sections[i];
		if(memcmp(section->sectname, "__text", 7) == 0) {
			relocations = (struct relocation_info *) (cxt->binary + section->reloff);
			relocation_count = section->nreloc;
			text_section = cxt->binary + section->offset;
			text_section_number = i + 1;
			break;
		}
	}

	if(!text_section) {
		return FALSE;
	}

	// perform relocation
	for(i = 0; i < relocation_count; i++) {
		struct relocation_info *reloc = &relocations[i];

		if(!(reloc->r_address & R_SCATTERED)) {
			void *symbol_address;
			void *target_address = text_section + reloc->r_address;
			if(reloc->r_extern) {
				struct nlist_64 *symbol = &symbols[reloc->r_symbolnum];
				const char *symbol_name = string_table + symbol->n_un.n_strx;
				symbol_address = qb_find_symbol(cxt, symbol_name);
				if(!symbol_address) {
					qb_abort("Missing symbol: %s\n", symbol_name);
					return FALSE;
				}
			} else {
				struct section_64 *section = &sections[reloc->r_symbolnum - 1];
				symbol_address = cxt->binary + section->offset;
			}

			int32_t S = (int32_t) symbol_address;
			int32_t P = ((int32_t) target_address) + sizeof(int64_t);

			if(reloc->r_type == X86_64_RELOC_BRANCH) {
				if(reloc->r_pcrel) {
					*((uint32_t *) target_address) = S - P;
				} else {
					*((uint32_t *) target_address) = S;
				}
			} else {
				return FALSE;
			}
		}
	}

	// find the compiled functions
	uint32_t count;
	for(i = 0; i < symtab_command->nsyms; i++) {
		struct nlist_64 *symbol = &symbols[i];
		if(symbol->n_sect != NO_SECT) {
			const char *symbol_name = string_table + symbol->n_un.n_strx;
			void *symbol_address = text_section + symbol->n_value;
			if(symbol->n_sect == text_section_number) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + 1, symbol_address);
				if(attached) {
					count += attached;
				} else {
					return FALSE;
				}
			} else {
				if(strncmp(symbol_name + 1, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}
	#else
static int32_t ZEND_FASTCALL qb_parse_macho32(qb_native_compiler_context *cxt) {
	struct mach_header *header;
	struct load_command *command;
	struct segment_command *segment_command;
	struct section *sections = NULL;
	struct symtab_command *symtab_command;
	struct nlist *symbols = NULL;
	char *string_table;
	struct relocation_info *relocations;
	uint32_t relocation_count;
	char *text_section = NULL;
	uint32_t text_section_number = 0;
	uint32_t i;

	if(cxt->binary_size < sizeof(struct mach_header)) {
		return FALSE;
	}

	header = (struct mach_header *) cxt->binary;
	if(header->magic != MH_MAGIC || header->filetype != MH_OBJECT) {
		return FALSE;
	}
		#ifdef __i386__
	if(header->cputype != CPU_TYPE_X86) {
		return FALSE;
	}
		#endif

	command = (struct load_command *) (cxt->binary + sizeof(struct mach_header));
	for(i = 0; i < header->ncmds; i++) {
		if(command->cmd == LC_SEGMENT) {
			// there's only one segment inside a mach-o object file
			// section structures of the segment are located right after the segment command
			segment_command = (struct segment_command *) command;
			sections = (struct section *) ((char *) command + sizeof(struct segment_command));
		} else if(command->cmd == LC_SYMTAB) {
			symtab_command = (struct symtab_command *) command;
			symbols = (struct nlist *) (cxt->binary + symtab_command->symoff);
			string_table = cxt->binary + symtab_command->stroff;
		}
		command = (struct load_command *) (((char *) command) + command->cmdsize);
	}

	if(!sections || !symbols) {
		return FALSE;
	}

	for(i = 0; i < segment_command->nsects; i++) {
		struct section *section = &sections[i];
		if(memcmp(section->sectname, "__text", 7) == 0) {
			relocations = (struct relocation_info *) (cxt->binary + section->reloff);
			relocation_count = section->nreloc;
			text_section = cxt->binary + section->offset;
			text_section_number = i + 1;
			break;
		}
	}

	if(!text_section) {
		return FALSE;
	}

	// perform relocation
	for(i = 0; i < relocation_count; i++) {
		struct relocation_info *reloc = &relocations[i];

		if(!(reloc->r_address & R_SCATTERED)) {
			void *symbol_address;
			void *target_address = text_section + reloc->r_address;
			if(reloc->r_extern) {
				struct nlist *symbol = &symbols[reloc->r_symbolnum];
				const char *symbol_name = string_table + symbol->n_un.n_strx;
				symbol_address = qb_find_symbol(cxt, symbol_name);
				if(!symbol_address) {
					qb_abort("Missing symbol: %s\n", symbol_name);
					return FALSE;
				}
			} else {
				struct section *section = &sections[reloc->r_symbolnum - 1];
				symbol_address = cxt->binary + section->offset;
			}

			int32_t S = (int32_t) symbol_address;
			int32_t P = ((int32_t) target_address) + sizeof(int32_t);

			if(reloc->r_type == GENERIC_RELOC_VANILLA && reloc->r_length == 2) {
				if(reloc->r_pcrel) {
					*((uint32_t *) target_address) = S - P;
				} else {
					*((uint32_t *) target_address) = S;
				}
			} else {
				return FALSE;
			}
		}
	}

	// find the compiled functions
	uint32_t count = 0;
	for(i = 0; i < symtab_command->nsyms; i++) {
		struct nlist *symbol = &symbols[i];
		if(symbol->n_sect != NO_SECT) {
			const char *symbol_name = string_table + symbol->n_un.n_strx;
			void *symbol_address = text_section + symbol->n_value;
			if(symbol->n_sect == text_section_number) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + 1, symbol_address);
				if(attached) {
					count += attached;
				} else {
					return FALSE;
				}
			} else {
				if(strncmp(symbol_name + 1, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}
	#endif	// __LP64__
#endif 	// __MACH__

#ifdef _MSC_VER
static int32_t ZEND_FASTCALL qb_parse_coff(qb_native_compiler_context *cxt) {
	IMAGE_FILE_HEADER *header;
	IMAGE_SECTION_HEADER *sections;
	IMAGE_SYMBOL *symbols;
	char *string_section;
	uint32_t count = 0;
	uint32_t i, j;

	if(cxt->binary_size < sizeof(IMAGE_FILE_HEADER)) {
		return FALSE;
	}

	header = (IMAGE_FILE_HEADER *) cxt->binary;
	if(header->Machine != IMAGE_FILE_MACHINE_I386 && header->Machine != IMAGE_FILE_MACHINE_IA64) {
		return FALSE;
	}
	sections = (IMAGE_SECTION_HEADER *) (((char *) header) + sizeof(IMAGE_FILE_HEADER));
	symbols = (IMAGE_SYMBOL *) (cxt->binary + header->PointerToSymbolTable);
	string_section = (char *) (symbols) + sizeof(IMAGE_SYMBOL) * header->NumberOfSymbols;

	// perform relocation
	for(i = 0; i < header->NumberOfSections; i++) {
		IMAGE_SECTION_HEADER *section = &sections[i];
		if(memcmp(section->Name, ".text", 6) == 0) {
			IMAGE_RELOCATION *relocations = (IMAGE_RELOCATION *) (cxt->binary + section->PointerToRelocations);
			uint32_t relocation_count = section->NumberOfRelocations;

			for(j = 0; j < relocation_count; j++) {
				IMAGE_RELOCATION *reloc = &relocations[j];
				IMAGE_SYMBOL *symbol = &symbols[reloc->SymbolTableIndex];
				char *symbol_name = (symbol->N.Name.Short) ? symbol->N.ShortName : string_section + symbol->N.Name.Long;
				void *symbol_address;
				void *target_address = cxt->binary + section->PointerToRawData + reloc->VirtualAddress;
				int32_t A, S, P; 

				if(symbol->SectionNumber == IMAGE_SYM_UNDEFINED) {
					symbol_address = qb_find_symbol(cxt, symbol_name);
					if(!symbol_address) {
						//qb_abort("Missing symbol: %s\n", symbol_name);
						zend_printf("Missing symbol: %s\n", symbol_name);
					}
				} else {
					// probably something in the data segment (e.g. a string literal)
					IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
					symbol_address = cxt->binary + section->PointerToRawData + symbol->Value;
				}

				A = *((int32_t *) target_address);
				S = (int32_t) symbol_address;
				P = ((int32_t) target_address) + sizeof(int32_t);

				switch(reloc->Type) {
					case IMAGE_REL_I386_DIR32:
						*((uint32_t *) target_address) = S + A;
						break;
					case IMAGE_REL_I386_REL32:
						*((uint32_t *) target_address) = S + A - P;
						break;
					default:
						return FALSE;
				}
			}
		}
	}

	// find the compiled functions
	for(i = 0; i < header->NumberOfSymbols; i++) {
		IMAGE_SYMBOL *symbol = &symbols[i];
		if(symbol->SectionNumber > 0) {
			IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
			char *symbol_name = (symbol->N.Name.Short) ? symbol->N.ShortName : (string_section + symbol->N.Name.Long);
			void *symbol_address = cxt->binary + section->PointerToRawData + symbol->Value;
			if(ISFCN(symbol->Type)) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + 1, symbol_address);
				if(!attached) {
					// error out if there's an unrecognized function
					if(!qb_find_symbol(cxt, symbol_name)) {
						return FALSE;
					}
				}
				count += attached;
			} else if(symbol->StorageClass == IMAGE_SYM_CLASS_EXTERNAL) {
				if(strncmp(symbol_name + 1, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}
#endif	// _MSC_VER

static zend_always_inline int32_t qb_parse_object_file(qb_native_compiler_context *cxt) {
#ifdef __ELF__
	#ifdef __LP64__
	return qb_parse_elf64(cxt);
	#else
	return qb_parse_elf32(cxt);
	#endif
#endif
#ifdef __MACH__
	#ifdef __LP64__
	return qb_parse_macho64(cxt);
	#else
	return qb_parse_macho32(cxt);
	#endif
#endif
#ifdef _MSC_VER
	return qb_parse_coff(cxt);
#endif
}

extern const char compressed_table_native_actions[];
extern const char compressed_table_native_result_size_possibilities[];
extern const char compressed_table_native_result_size_calculations[];
extern const char compressed_table_native_prototypes[];
extern const char compressed_table_native_references[];

static void ZEND_FASTCALL qb_initialize_context(qb_native_compiler_context *cxt TSRMLS_DC) {
	qb_build_context *build_cxt = QB_G(build_context);
	memset(cxt, 0, sizeof(qb_native_compiler_context));

	cxt->pool = build_cxt->pool;
	cxt->print_errors = QB_G(show_compiler_errors);
	cxt->print_source = QB_G(show_native_source);
	cxt->compiler_contexts = build_cxt->compiler_contexts;
	cxt->compiler_context_count = build_cxt->compiler_context_count;
	SAVE_TSRMLS();

	cxt->cache_folder_path = QB_G(native_code_cache_path);
}

static void ZEND_FASTCALL qb_free_context(qb_native_compiler_context *cxt) {
	USE_TSRM
	if(cxt->write_stream) {
		fclose(cxt->write_stream);
	}
	if(cxt->read_stream) {
		fclose(cxt->read_stream);
	}
	if(cxt->error_stream) {
		fclose(cxt->error_stream);
	}
	if(cxt->cache_folder_path && cxt->cache_folder_path != QB_G(native_code_cache_path)) {
		efree(cxt->cache_folder_path);
	}
	if(cxt->obj_file_path) {
		efree(cxt->obj_file_path);
	}
	if(cxt->c_file_path) {
		efree(cxt->c_file_path);
	}
#ifdef __GNUC__
	if(cxt->binary) {
		munmap(cxt->binary, cxt->binary_size);
	}
#endif
#ifdef _MSC_VER
	if(cxt->binary) {
		UnmapViewOfFile(cxt->binary);
	}
	if(cxt->msc_process) {
		CloseHandle(cxt->msc_process);
	}
	if(cxt->msc_thread) {
		CloseHandle(cxt->msc_thread);
	}
#endif
}

void ZEND_FASTCALL qb_free_native_code(qb_native_code_bundle *bundle) {
#ifdef __GNUC__
	munmap(bundle->memory, bundle->size);
#endif
#ifdef _MSC_VER
	UnmapViewOfFile(bundle->memory);
#endif
}

static void ZEND_FASTCALL qb_create_cache_folder(qb_native_compiler_context *cxt) {
	int len = strlen(cxt->cache_folder_path);
	if(len == 0) {
		// use the system temp folder when no cache folder is specified
#ifdef __GNUC__
		char *temp_folder = getenv("TMPDIR");
		if(!temp_folder) {
			temp_folder = "/tmp";
		}
		len = strlen(temp_folder);
		if(temp_folder[len - 1] == '/') {
			len--;
		}
		cxt->cache_folder_path = estrndup(temp_folder, len);
#endif
#ifdef _MSC_VER
		len = GetTempPath(0, NULL);
		cxt->cache_folder_path = emalloc(len + 1);
		GetTempPath(len + 1, cxt->cache_folder_path);
		cxt->cache_folder_path[len - 1] = '\0';
#endif
	}
#ifdef __GNUC__
	mkdir(cxt->cache_folder_path, 0777);
#endif
#ifdef _MSC_VER
	CreateDirectory(cxt->cache_folder_path, NULL);
#endif
}

#if ZEND_DEBUG
static void ZEND_FASTCALL qb_link_debuggable_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		for(j = 0; j < native_proc_table_size; j++) {
			qb_native_proc_record *rec = &native_proc_table[j];
			if(compiler_cxt->instruction_crc64 == rec->crc64) {
				compiler_cxt->native_proc = rec->proc;
			}
		}
	}
}
#endif

int32_t ZEND_FASTCALL qb_decompress_code(qb_native_compiler_context *cxt) {
	// decompress string tables used to generate the C source code
	if(!cxt->pool->op_actions) {
		qb_uncompress_table(compressed_table_native_actions, (void ***) &cxt->pool->op_actions, NULL, 0);
	}
	if(!cxt->pool->op_result_size_variables) {
		qb_uncompress_table(compressed_table_native_result_size_possibilities, (void ***) &cxt->pool->op_result_size_variables, NULL, 0);
	}
	if(!cxt->pool->op_result_size_codes) {
		qb_uncompress_table(compressed_table_native_result_size_calculations, (void ***) &cxt->pool->op_result_size_codes, NULL, 0);
	}
	if(!cxt->pool->op_function_usages) {
		qb_uncompress_table(compressed_table_native_references, (void ***) &cxt->pool->op_function_usages, NULL, 0);
	}
	if(!cxt->pool->function_prototypes) {
		uint32_t count;
		qb_uncompress_table(compressed_table_native_prototypes, (void ***) &cxt->pool->function_prototypes, &count, 0);
#if ZEND_DEBUG
		if(count > PROTOTYPE_COUNT) {
			qb_abort("Not enough space for the number of possible prototypes");
		}
#endif
	}

	cxt->op_names = cxt->pool->op_names;
	cxt->op_actions = cxt->pool->op_actions;
	cxt->op_result_size_codes = cxt->pool->op_result_size_codes;
	cxt->op_result_size_variables = cxt->pool->op_result_size_variables;
	cxt->op_function_usages = cxt->pool->op_function_usages;
	cxt->function_prototypes = cxt->pool->function_prototypes;

	return (cxt->op_actions && cxt->op_result_size_variables && cxt->op_result_size_codes && cxt->op_function_usages && cxt->function_prototypes);
}

int ZEND_FASTCALL qb_native_compile(TSRMLS_D) {
	int result = FAILURE;
	uint32_t i, attempt;
	qb_native_compiler_context _cxt, *cxt = &_cxt;

	qb_initialize_context(cxt TSRMLS_CC);

#if ZEND_DEBUG
	if(native_proc_table) {
		// link the functions to code in qb_native_proc_debug.c instead of compiling them live
		qb_link_debuggable_functions(cxt);
		return SUCCESS;
	}
#endif

	// make sure the cache folder exists
	qb_create_cache_folder(cxt);

	// XOR the crc64 of the functions together and use it as the file id
	cxt->file_id = 0;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		if(!compiler_cxt->native_proc && (compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE)) {
			cxt->file_id ^= compiler_cxt->instruction_crc64;
		}
	}
	spprintf(&cxt->obj_file_path, 0, "%s%cQB%" PRIX64 ".o", cxt->cache_folder_path, PHP_DIR_SEPARATOR, cxt->file_id);

#if ZEND_DEBUG
	for(attempt = 2; attempt <= 2; attempt++) {
#else
	for(attempt = 1; attempt <= 2; attempt++) {
#endif

		// first, try to load a previously created object file
		if(attempt == 2) {
			if(!qb_decompress_code(cxt)) {
				php_error_docref0(NULL TSRMLS_CC, E_WARNING, "Unable to decompress C source code");
				break;
			}

			// launch compiler
			if(!qb_launch_compiler(cxt)) {
				php_error_docref0(NULL TSRMLS_CC, E_WARNING, "Unable to launch compiler");
				break;
			}

#if ZEND_DEBUG
			// exclude macros, type declaration, and prototypes so they don't conflict with
			// what's defined in the header files if we include generated code to debug it
			qb_print(cxt, "#ifndef ZEND_DEBUG\n");
#endif
			// print macros and type definitions
			qb_print_macros(cxt);
			qb_print_typedefs(cxt);

			// print prototypes of function referenced
			qb_print_prototypes(cxt);
#if ZEND_DEBUG
	qb_print(cxt, "#endif\n");
#endif
			// print the current QB version
			qb_print_version(cxt);

			// print code of the qb functions themselves
			qb_print_functions(cxt);

#if ZEND_DEBUG
			// print a table of the functions
			qb_print_function_records(cxt);
#endif
			// wait for compiler to finish and see if it emits any error messages
			if(!qb_wait_for_compiler_response(cxt)) {
				break;
			}
		}

		// load the object file produced by the compiler into memory
		if(qb_load_object_file(cxt)) {
			// parse the object file and find pointers to the compiled functions
			if(qb_parse_object_file(cxt)) {
				if(cxt->qb_version == QB_VERSION_SIGNATURE) {
					qb_native_code_bundle *bundle;
					if(!QB_G(native_code_bundles)) {
						qb_create_array((void **) &QB_G(native_code_bundles), &QB_G(native_code_bundle_count), sizeof(qb_native_code_bundle), 8);
					}
					bundle = qb_enlarge_array((void **) &QB_G(native_code_bundles), 1);
					bundle->memory = cxt->binary;
					bundle->size = cxt->binary_size;
					qb_lock_object_file(cxt);
					cxt->binary = NULL;
					result = SUCCESS;
					break;
				} else {
					qb_detach_symbols(cxt);
				}
			}
			if(result != SUCCESS) {
				qb_remove_object_file(cxt);
			}
		}
	} 

	qb_free_context(cxt);
	return result;
}

#else

int ZEND_FASTCALL qb_native_compile(TSRMLS_D) {
	return FAILURE;
}

void ZEND_FASTCALL qb_free_native_code(void *bundle) {
}

#endif	// NATIVE_COMPILE_ENABLED
