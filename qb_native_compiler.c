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

static void qb_print(qb_native_compiler_context *cxt, const char *s) {
	USE_TSRM
	uint32_t len = strlen(s);
	if(cxt->print_source) {
		php_write((char *) s, len TSRMLS_CC);
	}
	fwrite(s, 1, len, cxt->write_stream);
}

#ifdef __GNUC__
static int32_t qb_launch_gcc(qb_native_compiler_context *cxt) {
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
#elif defined(__SSE4__)
		args[argc++] = "-msse4";
#elif defined(__SSE3__)
		args[argc++] = "-msse3";
#elif defined(__SSE2__)
		args[argc++] = "-msse2";
#elif defined(__SSE__)
		args[argc++] = "-msse";
#endif
		args[argc++] = "-pipe";										// use pipes for internal communication
#if !ZEND_DEBUG
		args[argc++] = "-Wp,-w";									// disable preprocessor warning
#endif
		args[argc++] = "-Werror=implicit-function-declaration";		// elevate implicit function declaration to an error
		args[argc++] = "-fno-stack-protector"; 						// disable stack protector
#if defined(__LP64__) && !defined(__APPLE__)
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
static int32_t qb_launch_cl(qb_native_compiler_context *cxt) {
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

static void qb_print_macros(qb_native_compiler_context *cxt) {
	qb_print(cxt, "#define NAN	"STRING(NAN)"\n");
	qb_print(cxt, "#define INF	"STRING(INFINITY)"\n");
	qb_print(cxt, "#define restrict	__restrict\n");
	qb_print(cxt, "#define NULL	"STRING(NULL)"\n");
	qb_print(cxt, "#define M_PI	"STRING(M_PI)"\n");
	qb_print(cxt, "#define MAX_DIMENSION	"STRING(MAX_DIMENSION)"\n");
#ifdef FAST_FLOAT_TO_INT
	qb_print(cxt, "#define FAST_FLOAT_TO_INT\n");
#endif
#ifdef QB_LITTLE_ENDIAN
	qb_print(cxt, "#define QB_LITTLE_ENDIAN\n");
#else
	qb_print(cxt, "#define QB_BIG_ENDIAN\n");
#endif
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
	qb_print(cxt, "#define EXPECTED(c)	"STRING(EXPECTED(c))"\n");
	qb_print(cxt, "#define UNEXPECTED(c)	"STRING(UNEXPECTED(c))"\n");
	qb_print(cxt, "#define zend_always_inline	"STRING(zend_always_inline)"\n");
	qb_print(cxt, "#define SWAP_BE_I16(v)	"STRING(SWAP_BE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I32(v)	"STRING(SWAP_BE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I64(v)	"STRING(SWAP_BE_I64(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I16(v)	"STRING(SWAP_LE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I32(v)	"STRING(SWAP_LE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I64(v)	"STRING(SWAP_LE_I64(v))"\n");
	qb_print(cxt, "#define TRUE	"STRING(TRUE)"\n");
	qb_print(cxt, "#define FALSE	"STRING(FALSE)"\n");

#ifdef __GNUC__
#ifndef __builtin_bswap16
	qb_print(cxt, "#define __builtin_bswap16(n)	((n >> 8) | (n << 8))\n");
#endif
#endif
}

static void qb_print_typedefs(qb_native_compiler_context *cxt) {
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
#if SIZEOF_PTRDIFF_T == 8
	qb_print(cxt, "typedef int64_t intptr_t;\n");
	qb_print(cxt, "typedef uint64_t uintptr_t;\n");
#else
	qb_print(cxt, "typedef int32_t intptr_t;\n");
	qb_print(cxt, "typedef uint32_t uintptr_t;\n");
#endif

	qb_print(cxt, "typedef float float32_t;\n");
	qb_print(cxt, "typedef double float64_t;\n");
	qb_print(cxt, "\n");

	qb_print(cxt, "typedef struct qb_address\tqb_address;\n");
	qb_print(cxt, "typedef struct qb_variable\tqb_variable;\n");
	qb_print(cxt, "typedef struct zend_op_array\tzend_op_array;\n");
	qb_print(cxt, "typedef struct zval\tzval;\n");
	qb_print(cxt, "typedef struct php_stream\tphp_stream;\n");
	qb_print(cxt, "typedef struct qb_index_alias_scheme\tqb_index_alias_scheme;\n");
	qb_print(cxt, "typedef struct qb_memory_segment\tqb_memory_segment;\n");
	qb_print(cxt, "typedef struct qb_storage\tqb_storage;\n");
	qb_print(cxt, "typedef struct qb_function\tqb_function;\n");
	qb_print(cxt, "typedef struct qb_interpreter_context\tqb_interpreter_context;\n");
	qb_print(cxt, "typedef struct qb_external_symbol\tqb_external_symbol;\n");
	qb_print(cxt, "typedef struct qb_thread_worker\tqb_thread_worker;\n");
	
	qb_print(cxt, "typedef enum qb_vm_exit_type\tqb_vm_exit_type;\n");

	qb_print(cxt, "\
enum qb_vm_exit_type {\
	QB_VM_RETURN = 0,\
	QB_VM_BAILOUT,\
	QB_VM_TIMEOUT,\
	QB_VM_FORK,\
	QB_VM_SPOON,\
	QB_VM_EXCEPTION,\
	QB_VM_YIELD,\
};\
\n");

	qb_print(cxt, "\
struct qb_memory_segment {\
	int8_t *memory;\
	uint32_t flags;\
	uint32_t byte_count;\
	uint32_t current_allocation;\
	php_stream *stream;\
	qb_memory_segment *imported_segment;\
	qb_memory_segment *next_dependent;\
	uintptr_t **references;\
	uint32_t reference_count;\
};\
\n");

	qb_print(cxt, "\
struct qb_storage {\
	uint32_t size;\
	qb_memory_segment *segments;\
	uint32_t segment_count;\
	uint32_t flags;\
};\
\n");

	qb_print(cxt, "\
struct qb_external_symbol {\
	long hash_value;\
	const char *name;\
	void *address;\
};\
\n");

	qb_print(cxt, "\
struct qb_function {\
	int8_t *instructions;\
	int8_t *instruction_start;\
	uint64_t instruction_crc64;\
	uint32_t instruction_length;\
	uint16_t *instruction_opcodes;\
	uint32_t instruction_opcode_count;\
	uint32_t flags;\
	qb_variable *return_variable;\
	qb_variable **variables;\
	uint32_t variable_count;\
	uint32_t argument_count;\
	qb_storage *local_storage;\
	const char *name;\
	const char *filename;\
	void *native_proc;\
	uintptr_t instruction_base_address;\
	uintptr_t local_storage_base_address;\
	zend_op_array *zend_op_array;\
	qb_function *next_reentrance_copy;\
	qb_function *next_forked_copy;\
	volatile int32_t in_use;\
};\
\n");

	qb_print(cxt, "\
struct qb_interpreter_context {\
	qb_function *function;\
	int8_t *instruction_pointer;\
	qb_interpreter_context *caller_context;\
	qb_thread_worker *worker;\
	uint32_t worker_count;\
	uint32_t fork_id;\
	uint32_t fork_count;\
	uint32_t *argument_indices;\
	uint32_t argument_count;\
	uint32_t result_index;\
	uint32_t line_number;\
	uint32_t call_depth;\
	qb_vm_exit_type exit_type;\
	int32_t exit_status_code;\
	int32_t exception_encountered;\
	volatile unsigned char *windows_timed_out_pointer;\
	int floating_point_precision;\
	void ***tsrm_ls;\
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

#define PROTOTYPE_COUNT		2000

static void qb_print_prototypes(qb_native_compiler_context *cxt) {
	uint32_t i, j, k;
	int32_t *prototype_indices, index;
	uint32_t required[PROTOTYPE_COUNT];
	qb_op *qop;

	// see which functions are required
	memset(required, 0, sizeof(required));
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			// go through all ops and see what functions they use
			for(j = 0; j < compiler_cxt->op_count; j++) {
				qop = compiler_cxt->ops[j];
				prototype_indices = cxt->op_function_usages[qop->opcode];
				for(k = 0; prototype_indices[k] != 0xFFFFFFFF; k++) {
					index = prototype_indices[k];
					required[index] = TRUE;
				}
			}

		}
	}

	// print prototypes, maintaining correct order
	for(i = 0; i < PROTOTYPE_COUNT; i++) {
		if(required[i]) {
			const char *prototype = cxt->function_prototypes[i];
			qb_print(cxt, prototype);
			qb_print(cxt, "\n");
		}
	}
	qb_print(cxt, "\n");
}

static char * qb_get_buffer(qb_native_compiler_context *cxt) {
	if(cxt->string_buffer_index == 16) {
		cxt->string_buffer_index = 0;
	}
	return cxt->string_buffers[cxt->string_buffer_index++];
}

static qb_address * qb_get_root_address(qb_native_compiler_context *cxt, qb_address *address) {
	while(address->source_address) {
		address = address->source_address;
	}
	return address;
}

static int32_t qb_cast_required(qb_native_compiler_context *cxt, qb_address *address) {
	qb_address *base_address = qb_get_root_address(cxt, address);
	return (base_address->type != address->type);
}

static int32_t qb_offset_required(qb_native_compiler_context *cxt, qb_address *address) {
	if(address->array_index_address != cxt->zero_address) {
		return TRUE;
	} else {
		qb_address *base_address = qb_get_root_address(cxt, address);
		return (base_address->segment_offset != address->segment_offset);
	}
}

static qb_access_method qb_get_scalar_access_method(qb_native_compiler_context *cxt, qb_address *address) {
	switch(address->segment_selector) {
		case QB_SELECTOR_CONSTANT_ARRAY:
		case QB_SELECTOR_CONSTANT_SCALAR: return QB_SCALAR_LITERAL;
		case QB_SELECTOR_CLASS_SCALAR:
		case QB_SELECTOR_OBJECT_SCALAR:
		case QB_SELECTOR_GLOBAL_SCALAR:
		case QB_SELECTOR_STATIC_SCALAR:
		case QB_SELECTOR_SHARED_SCALAR: return QB_SCALAR_POINTER;
		case QB_SELECTOR_LOCAL_SCALAR:
		case QB_SELECTOR_TEMPORARY_SCALAR: {
			if(address->flags & QB_ADDRESS_DIMENSION) {
				return QB_SCALAR_POINTER;
			} else {
				return QB_SCALAR_LOCAL_VARIABLE;
			}
		}
		default: return QB_SCALAR_ELEMENT;
	}
}

static qb_access_method qb_get_array_access_method(qb_native_compiler_context *cxt, qb_address *address) {
	switch(address->segment_selector) {
		case QB_SELECTOR_CONSTANT_SCALAR:
		case QB_SELECTOR_CLASS_SCALAR:
		case QB_SELECTOR_OBJECT_SCALAR:
		case QB_SELECTOR_GLOBAL_SCALAR:
		case QB_SELECTOR_STATIC_SCALAR:
		case QB_SELECTOR_SHARED_SCALAR: return QB_SCALAR_POINTER;
		case QB_SELECTOR_LOCAL_SCALAR:
		case QB_SELECTOR_TEMPORARY_SCALAR: {
			if(address->flags & QB_ADDRESS_DIMENSION) {
				return QB_SCALAR_POINTER;
			} else {
				return QB_SCALAR_LOCAL_VARIABLE;
			}
		}
		case QB_SELECTOR_CONSTANT_ARRAY:
		case QB_SELECTOR_CLASS_ARRAY:
		case QB_SELECTOR_OBJECT_ARRAY:
		case QB_SELECTOR_GLOBAL_ARRAY:
		case QB_SELECTOR_SHARED_ARRAY: 
		case QB_SELECTOR_LOCAL_ARRAY:
		case QB_SELECTOR_TEMPORARY_ARRAY: {
			if(qb_offset_required(cxt, address)) {
				return QB_ARRAY_SLICE;
			} else {
				return QB_ARRAY_POINTER;
			}
		}
		default: {
			if(qb_offset_required(cxt, address)) {
				return QB_ARRAY_SLICE;
			} else {
				return QB_ARRAY_POINTER_POINTER;
			}
		}
	}
}

static const char * qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address);

static const char * qb_get_scalar(qb_native_compiler_context *cxt, qb_address *address) {
	char *buffer = qb_get_buffer(cxt);
	const char *ctype = qb_cast_required(cxt, address) ? type_cnames[address->type] : NULL;
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	switch(method) {
		case QB_SCALAR_LITERAL: {
			// scalar is a constant
			switch(address->type) {
				case QB_TYPE_S08: snprintf(buffer, 128, "%" PRId8, VALUE(S08, address)); break;
				case QB_TYPE_U08: snprintf(buffer, 128, "%" PRIu8"U", VALUE(U08, address)); break;
				case QB_TYPE_S16: snprintf(buffer, 128, "%" PRId16, VALUE(S16, address)); break;
				case QB_TYPE_U16: snprintf(buffer, 128, "%" PRIu16"U", VALUE(U16, address)); break;
				case QB_TYPE_S32: snprintf(buffer, 128, "%" PRId32, VALUE(S32, address)); break;
				case QB_TYPE_U32: snprintf(buffer, 128, "%" PRIu32"U", VALUE(U32, address)); break;
				case QB_TYPE_S64: snprintf(buffer, 128, "%" PRId64"LL", VALUE(S64, address)); break;
				case QB_TYPE_U64: snprintf(buffer, 128, "%" PRIu64"ULL", VALUE(U64, address)); break;
				case QB_TYPE_F32: {
					float32_t number = VALUE(F32, address);
					if(isnan(number)) {
						snprintf(buffer, 128, "NAN");
					} else if(zend_finite(number)) {
						snprintf(buffer, 128, "%.11ff", number); 
					} else {
						if(number > 0) {
							snprintf(buffer, 128, "INF");
						} else {
							snprintf(buffer, 128, "-INF");
						}
					}
				}	break;
				case QB_TYPE_F64: {
					float64_t number = VALUE(F64, address);
					if(isnan(number)) {
						snprintf(buffer, 128, "NAN");
					} else if(zend_finite(number)) {
						snprintf(buffer, 128, "%.17f", number); 
					} else {
						if(number > 0) {
							snprintf(buffer, 128, "INF");
						} else {
							snprintf(buffer, 128, "-INF");
						}
					}
				}	break;
				default: break;
			}
		}	break;
		case QB_SCALAR_POINTER: {
			// scalar is referenced by pointer
			if(ctype) {
				snprintf(buffer, 128, "((%s) (*var_ptr_%d_%d))", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "(*var_ptr_%d_%d)", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_LOCAL_VARIABLE: {
			// scalar is a local variable
			if(ctype) {
				snprintf(buffer, 128, "((%s) var_%d_%d)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_%d_%d", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_ELEMENT: {
			// array elements 
			qb_address *base_address = qb_get_root_address(cxt, address);
			const char *container = qb_get_pointer(cxt, base_address);
			if(address->array_index_address == cxt->zero_address) {
				// referenced by a fixed index
				uint32_t index = ELEMENT_COUNT(address->segment_offset - base_address->segment_offset, address->type);
				if(ctype) {
					snprintf(buffer, 128, "((%s) %s[%d])", ctype, container, index);
				} else {
					snprintf(buffer, 128, "(%s[%d])", container, index);
				}
			} else {
				// variable index
				const char *index = qb_get_scalar(cxt, address->array_index_address);
				if(ctype) {
					snprintf(buffer, 128, "((%s) %s[%s])", ctype, container, index);
				} else {
					snprintf(buffer, 128, "(%s[%s])", container, index);
				}
			}
		}	break;
		default: {
		}	break;
	}
	return buffer;
}

static const char * qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address) {
	char *buffer = qb_get_buffer(cxt);
	const char *ctype = qb_cast_required(cxt, address) ? type_cnames[address->type] : NULL;
	qb_access_method method = qb_get_array_access_method(cxt, address);
	switch(method) {
		case QB_SCALAR_POINTER: {
			if(ctype) {
				snprintf(buffer, 128, "((%s *) var_ptr_%d_%d)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_ptr_%d_%d", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_LOCAL_VARIABLE: {
			if(ctype) {
				snprintf(buffer, 128, "((%s *) &var_%d_%d)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "&var_%d_%d", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_ARRAY_POINTER: {
			// fixed-length array
			if(ctype) {
				snprintf(buffer, 128, "((%s *) var_ptr_%d_%d)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_ptr_%d_%d", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_ARRAY_POINTER_POINTER: {
			// a variable-length array--dereference the pointer to pointer
			if(ctype) {
				snprintf(buffer, 128, "((%s *) (*var_ptr_ptr_%d))", ctype, address->segment_selector);
			} else {
				snprintf(buffer, 128, "(*var_ptr_ptr_%d)", address->segment_selector);
			}
		}	break;
		case QB_ARRAY_SLICE: {
			qb_address *base_address = qb_get_root_address(cxt, address);
			const char *container = qb_get_pointer(cxt, base_address);
			if(address->array_index_address == cxt->zero_address) {
				uint32_t offset = ELEMENT_COUNT(address->segment_offset - base_address->segment_offset, address->type);
				if(ctype) {
					snprintf(buffer, 128, "(((%s *) %s) + %d)", ctype, container, offset);
				} else {
					snprintf(buffer, 128, "(%s + %d)", container, offset);
				}
			} else {
				const char *offset = qb_get_scalar(cxt, address->array_index_address);
				if(ctype) {
					snprintf(buffer, 128, "(((%s *) %s) + %s)", ctype, container, offset);
				} else {
					snprintf(buffer, 128, "(%s + %s)", container, offset);
				}
			}
		}	break;
		default: {
		}	break;
	}
	return buffer;
}

static const char * qb_get_jump_label(qb_native_compiler_context *cxt, uint32_t jump_target_index) {
	char *buffer = qb_get_buffer(cxt);
	snprintf(buffer, 128, "L%04d", jump_target_index);
	return buffer;
}

static const char * qb_get_op_action(qb_native_compiler_context *cxt, uint32_t opcode) {
	return cxt->op_actions[opcode];
}

extern const char compressed_table_op_names[];

static const char * qb_get_op_name(qb_native_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->op_names) {
		// decompress the opname table
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		if(!cxt->pool->op_names) {
			return "?";
		}
	}
	return cxt->pool->op_names[opcode];
}

static void qb_copy_local_scalar_to_storage(qb_native_compiler_context *cxt, qb_address *address) {
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	if(method == QB_SCALAR_LOCAL_VARIABLE) {
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "*((%s *) (storage->segments[%d].memory + %d)) = var_%d_%d;\n", c_type, address->segment_selector, address->segment_offset, address->segment_selector, address->segment_offset);
	}
}

static void qb_copy_local_variables_to_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t i;
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			qb_address *address = operand->address;
			switch(address->mode) {
				case QB_ADDRESS_MODE_SCA: {
					qb_copy_local_scalar_to_storage(cxt, address);
				}	break;
				case QB_ADDRESS_MODE_ELE: {
					qb_copy_local_scalar_to_storage(cxt, address->array_index_address);
				}	break;
				case QB_ADDRESS_MODE_ARR: {
					qb_copy_local_scalar_to_storage(cxt, address->array_index_address);
				}	break;
			}
		}
	}
}

static void qb_copy_local_arguments_to_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t *var_indices = ARRAY(U32, qop->operands[1].address);
	uint32_t arg_count = ARRAY_SIZE(qop->operands[1].address);
	uint32_t i;
	for(i = 0; i < arg_count; i++) {
		qb_variable *qvar = cxt->variables[var_indices[i]];
		qb_address *address = qvar->address;
		switch(address->mode) {
			case QB_ADDRESS_MODE_SCA: {
				qb_copy_local_scalar_to_storage(cxt, address);
			}	break;
		}
	}
}

static void qb_copy_local_scalar_from_storage(qb_native_compiler_context *cxt, qb_address *address) {
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	if(method == QB_SCALAR_LOCAL_VARIABLE) {
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "var_%d_%d = *((%s *) (storage->segments[%d].memory + %d));\n", address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
	}
}

static void qb_copy_local_variables_from_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t i;
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(qb_is_operand_write_target(qop->opcode, i)) {
				qb_address *address = operand->address;
				switch(address->mode) {
					case QB_ADDRESS_MODE_SCA: {
						qb_copy_local_scalar_from_storage(cxt, address);
					}	break;
					case QB_ADDRESS_MODE_ELE: {
						qb_copy_local_scalar_from_storage(cxt, address->array_index_address);
					}	break;
					case QB_ADDRESS_MODE_ARR: {
						qb_copy_local_scalar_from_storage(cxt, address->array_index_address);
					}	break;
				}
			}
		}
	}
}

static void qb_copy_local_arguments_from_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	USE_TSRM
	uint32_t symbol_index = VALUE(U32, qop->operands[0].address);
	qb_external_symbol *symbol = &QB_G(external_symbols)[symbol_index];
	zend_function *zfunc = symbol->pointer;
	qb_function *qfunc = qb_get_compiled_function(zfunc);
	uint32_t *var_indices = ARRAY(U32, qop->operands[1].address);
	uint32_t arg_count = ARRAY_SIZE(qop->operands[1].address);
	uint32_t retval_index = VALUE(U32, qop->operands[2].address);
	uint32_t i;
	for(i = 0; i < arg_count; i++) {
		int32_t by_ref;
		if(qfunc) {
			if(i < qfunc->argument_count ) {
				qb_variable *arg = qfunc->variables[i];
				by_ref = (arg->flags & QB_VARIABLE_BY_REF);
			} else {
				by_ref = FALSE;
			}
		} else {
			if(zfunc->common.arg_info && i < zfunc->common.num_args) {
				by_ref = zfunc->common.arg_info[i].pass_by_reference;
			} else {
				by_ref = TRUE;
			}
		}
		if(by_ref) {
			qb_variable *qvar = cxt->variables[var_indices[i]];
			qb_address *address = qvar->address;
			switch(address->mode) {
				case QB_ADDRESS_MODE_SCA: {
					qb_copy_local_scalar_from_storage(cxt, address);
				}	break;
			}
		}
	}
	if(retval_index != INVALID_INDEX) {
		qb_variable *qvar = cxt->variables[retval_index];
		qb_address *address = qvar->address;
		switch(address->mode) {
			case QB_ADDRESS_MODE_SCA: {
				qb_copy_local_scalar_from_storage(cxt, address);
			}	break;
		}
	}
}

static void qb_print_op(qb_native_compiler_context *cxt, qb_op *qop, uint32_t qop_index) {
	if(qop->flags & QB_OP_JUMP_TARGET) {
		const char *label = qb_get_jump_label(cxt, qop_index);
		qb_printf(cxt, "%s:\n", label);
	}
	if(qop->opcode != QB_NOP) {
		const char *name;
		const char *action;
		uint32_t i, j;

		if(cxt->print_source || TRUE) {
			name = qb_get_op_name(cxt, qop->opcode);
			qb_printf(cxt, "// %s (line #%d)\n", name, qop->line_number);
		}

		// define the operands
		if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
			qb_printf(cxt, "#define line_number	%d\n", qop->line_number);
		}
		for(i = 0; i < qop->operand_count; i++) {
			qb_operand *operand = &qop->operands[i];
			if(operand->type == QB_OPERAND_ADDRESS) {
				qb_address *address = operand->address;
				char name[4];
				if(i == qop->operand_count - 1 && qb_is_operand_write_target(qop->opcode, i)) {
					sprintf(name, "res");
				} else {
					sprintf(name, "op%d", i + 1);
				}
				switch(address->mode) {
					case QB_ADDRESS_MODE_SCA:
					case QB_ADDRESS_MODE_ELE: {
						const char *scalar = qb_get_scalar(cxt, address);
						qb_printf(cxt, "#define %s	%s\n", name, scalar);
					}	break;
					case QB_ADDRESS_MODE_ARR: {
						if(address->array_size_address != cxt->zero_address) {
							const char *count = qb_get_scalar(cxt, address->array_size_address);
							const char *pointer = qb_get_pointer(cxt, address);
							const char *count_pointer = qb_get_pointer(cxt, address->array_size_address);
							qb_printf(cxt, "#define %s_ptr	%s\n", name, pointer);
							qb_printf(cxt, "#define %s_count	%s\n", name, count);
							qb_printf(cxt, "#define %s_count_ptr	%s\n", name, count_pointer);
						} else {
							qb_printf(cxt, "#define %s_ptr	NULL\n", name);
							qb_printf(cxt, "#define %s_count	0U\n", name);
						}
					}	break;
				}
			}
		}
		qb_print(cxt, "\n");

		if(qop->flags & QB_OP_NEED_INSTRUCTION_STRUCT) {
			qb_copy_local_variables_to_storage(cxt, qop);
			qb_printf(cxt, "ip = cxt->function->instructions + %d;\n", qop->instruction_offset);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_RET) {
			if(cxt->compiled_function->return_variable->address) {
				if(SCALAR(cxt->compiled_function->return_variable->address)) {
					qb_copy_local_scalar_to_storage(cxt, cxt->compiled_function->return_variable->address);
					qb_print(cxt, "\n");
				}
			}
		} else if(qop->opcode == QB_FCALL_U32_U32_U32) {
			qb_copy_local_arguments_to_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_END_STATIC) {
			qb_printf(cxt, "ip = cxt->function->instructions + %d;\n", qop->instruction_offset);
		}

		// print code that actually performs the action
		action = qb_get_op_action(cxt, qop->opcode);
		if(action) {
			qb_print(cxt, action);
			qb_print(cxt, "\n");
		}

		if(qop->flags & QB_OP_NEED_INSTRUCTION_STRUCT) {
			qb_copy_local_variables_from_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_FCALL_U32_U32_U32) {
			qb_copy_local_arguments_from_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_END_STATIC) {
			if(!(cxt->ops[qop_index]->flags & QB_OP_JUMP_TARGET)) {
				const char *jump_target = qb_get_jump_label(cxt, qop_index + 1);
				qb_printf(cxt, "%s:\n", jump_target);
			}
		}

#ifdef ZEND_WIN32				
		// check for timed-out condition on any backward jump
		for(j = 0; j < qop->jump_target_count; j++) {
			if(qop->jump_target_indices[j] <= qop_index) {
				qb_print(cxt, "if(*cxt->windows_timed_out_pointer) {\n");
				qb_print(cxt,		"zend_timeout(1);\n");
				qb_print(cxt,		"return QB_VM_TIMEOUT;\n");
				qb_print(cxt, "}\n");
				break;
			}
		}
#endif
		if(qop->flags & QB_OP_BRANCH) {
			if(qop->jump_target_indices[1] == qop_index + 1) {
				const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
				qb_printf(cxt, "if(condition) goto %s;\n", jump_target);
			} else if(qop->jump_target_indices[0] == qop_index + 1) {
				const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[1]);
				qb_printf(cxt, "if(!condition) goto %s;\n", jump_target);
			} else {
				const char *jump_target1 = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
				const char *jump_target2 = qb_get_jump_label(cxt, qop->jump_target_indices[1]);
				qb_printf(cxt, "if(condition) goto %s; else goto %s;\n", jump_target1, jump_target2);
			}
			qb_print(cxt, "\n");
		} else if(qop->flags & QB_OP_JUMP) {
			const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
			qb_printf(cxt, "goto %s;\n", jump_target);
			qb_print(cxt, "\n");
		}
	}
}

static void qb_print_local_variables(qb_native_compiler_context *cxt) {
	uint32_t i, j, k;
	int32_t *constant_as_pointer;
	ALLOCA_FLAG(use_heap);

	qb_print(cxt, "int8_t *ip;\n");
	qb_print(cxt, "int condition;\n");
	qb_print(cxt, "qb_storage *storage = cxt->function->local_storage;\n");

	// create variables for writable scalars
	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		const char *c_type = type_cnames[address->type];
		qb_access_method method = qb_get_scalar_access_method(cxt, address);
		switch(method) {
			case QB_SCALAR_POINTER: {
				// create pointers to the value in the storage
				qb_printf(cxt, "%s *var_ptr_%d_%d = (%s *) (storage->segments[%d].memory + %d);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
			case QB_SCALAR_LOCAL_VARIABLE: {
				// create local variables
				qb_printf(cxt, "%s var_%d_%d = *((%s *) (storage->segments[%d].memory + %d));\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
		}
	}
	qb_print(cxt, "\n");

	// create variables for writable arrays
	for(i = 0; i < cxt->writable_array_count; i++) {
		qb_address *address = cxt->writable_arrays[i];
		const char *c_type = type_cnames[address->type];
		qb_access_method method = qb_get_array_access_method(cxt, address);
		switch(method) {
			case QB_ARRAY_POINTER: {
				// create pointers to the value in the storage
				qb_printf(cxt, "%s *var_ptr_%d_%d = (%s *) (storage->segments[%d].memory + %d);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
			case QB_ARRAY_POINTER_POINTER: {
				// create pointers to pointers in the storage, as the realocation can occur for variable-length arrays
				qb_printf(cxt, "%s **var_ptr_ptr_%d = (%s **) &storage->segments[%d].memory;\n", c_type, address->segment_selector, c_type, address->segment_selector);
			}	break;
			default: {
			}	break;
		}
	}
	qb_print(cxt, "\n");

	// see if any constant scalar is referenced as arrays
	constant_as_pointer = do_alloca(cxt->constant_scalar_count * sizeof(int32_t), use_heap);
	memset(constant_as_pointer, 0, cxt->constant_scalar_count * sizeof(int32_t));
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *op = cxt->ops[i];
		for(j = 0; j < op->operand_count; j++) {
			qb_operand *operand = &op->operands[j];
			if(operand->type == QB_OPERAND_ADDRESS) {
				if(operand->address->mode == QB_ADDRESS_MODE_ARR && operand->address->segment_selector == QB_SELECTOR_CONSTANT_SCALAR) {
					for(k = 0; k < cxt->constant_scalar_count; k++) {
						if(operand->address->segment_offset == cxt->constant_scalars[k]->segment_offset) {
							constant_as_pointer[k] = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		if(constant_as_pointer[i]) {
			qb_address *address = cxt->constant_scalars[i];
			const char *c_type = type_cnames[address->type];
			qb_printf(cxt, "%s *var_ptr_%d_%d = (%s *) (storage->segments[%d].memory + %d);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
		}
	}
	free_alloca(constant_as_pointer, use_heap);

	// create variables for constant arrays
	for(i = 0; i < cxt->constant_array_count; i++) {
		qb_address *address = cxt->constant_arrays[i];
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "%s *var_ptr_%d_%d = (%s *) (storage->segments[%d].memory + %d);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
	}
	qb_print(cxt, "\n");

#ifdef ZTS
	qb_print(cxt, STRING(USE_TSRM)"\n");
#endif
	qb_print(cxt, "\n");
}

static void qb_print_reentry_switch(qb_native_compiler_context *cxt) {
	int32_t started = FALSE;
	uint32_t i;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		uint32_t restore_op_index = INVALID_INDEX;
		switch(qop->opcode) {
			case QB_END_STATIC: {
				restore_op_index = i + 1;
			}	break;
		}
		if(restore_op_index != INVALID_INDEX) {
			const char *jump_target = qb_get_jump_label(cxt, restore_op_index);
			if(!started) {
				qb_print(cxt, "switch(cxt->function->instruction_start - cxt->function->instructions) {\n");
				started = TRUE;
			}
			qb_printf(cxt,	"case %d: goto %s;\n", qop->instruction_offset, jump_target);
		}
	}
	if(started) {
		qb_print(cxt, "}\n");
		qb_print(cxt, "\n");
	}
}

static void qb_print_function(qb_native_compiler_context *cxt) {
	uint32_t i;
	if(cxt->print_source) {
		qb_printf(cxt, "\n// Handler for %s()\n", cxt->compiled_function->name);
	}
	qb_printf(cxt, STRING(QB_NATIVE_FUNCTION_RET QB_NATIVE_FUNCTION_ATTR) " QBN_%" PRIX64 "(" STRING(QB_NATIVE_FUNCTION_ARGS) ")\n{\n", cxt->compiled_function->instruction_crc64);
	qb_print_local_variables(cxt);
	qb_print_reentry_switch(cxt);
	for(i = 0; i < cxt->op_count; i++) {
		qb_print_op(cxt, cxt->ops[i], i);
	}
	qb_print(cxt, "}\n");
}

static void qb_print_version(qb_native_compiler_context *cxt) {
	qb_printf(cxt, "\nuint32_t QB_VERSION = 0x%08x;\n\n", QB_VERSION_SIGNATURE);
}

static void qb_print_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			// check if an earlier function has the same crc64
			int32_t duplicate = FALSE;
			for(j = 0; j < i; j++) {
				if(cxt->compiler_contexts[j]->compiled_function->instruction_crc64 == compiler_cxt->compiled_function->instruction_crc64) {
					duplicate = TRUE;
					break;
				}
			}
			if(!duplicate) {
				// copy variables from the compiler context to make life easier
				cxt->ops = compiler_cxt->ops;
				cxt->op_count = compiler_cxt->op_count;
				cxt->variables = compiler_cxt->variables;
				cxt->variable_count = compiler_cxt->variable_count;
				cxt->constant_scalars = compiler_cxt->constant_scalars;
				cxt->constant_scalar_count = compiler_cxt->constant_scalar_count;
				cxt->writable_scalars = compiler_cxt->writable_scalars;
				cxt->writable_scalar_count = compiler_cxt->writable_scalar_count;
				cxt->constant_arrays = compiler_cxt->constant_arrays;
				cxt->constant_array_count = compiler_cxt->constant_array_count;
				cxt->writable_arrays = compiler_cxt->writable_arrays;
				cxt->writable_array_count = compiler_cxt->writable_array_count;
				cxt->zero_address = compiler_cxt->zero_address;
				cxt->one_address = compiler_cxt->one_address;
				cxt->storage = compiler_cxt->storage;
				cxt->compiled_function = compiler_cxt->compiled_function;

				// print the function
				qb_print_function(cxt);
			}
		}
	}
}

static void qb_print_function_records(qb_native_compiler_context *cxt) {
	uint32_t i;
	qb_print(cxt, "\n");
	qb_print(cxt, "#define HAVE_NATIVE_PROC_RECORDS\n");
	qb_print(cxt, "qb_native_proc_record native_proc_records[] = {\n");
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			qb_printf(cxt, "	{ 0x%" PRIX64 "ULL, QBN_%" PRIX64 " },\n", compiler_cxt->compiled_function->instruction_crc64, compiler_cxt->compiled_function->instruction_crc64);
		}
	}
	qb_print(cxt, "};\n");
}

static void * qb_find_symbol(qb_native_compiler_context *cxt, const char *name) {
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
static int32_t qb_wait_for_compiler_response(qb_native_compiler_context *cxt) {
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
static int32_t qb_wait_for_compiler_response(qb_native_compiler_context *cxt) {
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
	//DeleteFile(cxt->c_file_path);
	return TRUE;
}
#endif	// _MSC_VER

#ifdef __GNUC__
static int32_t qb_load_object_file(qb_native_compiler_context *cxt) {
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
#ifndef __APPLE__			
			binary = mmap(NULL, binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_32BIT, file_descriptor, 0);
#else
			binary = mmap((void *) ~0, binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, file_descriptor, 0);
#endif			
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

static void qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		munmap(cxt->binary, cxt->binary_size);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	unlink(cxt->obj_file_path);
}

#endif	// __GNUC__

#ifdef _MSC_VER
static int32_t qb_load_object_file(qb_native_compiler_context *cxt) {
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

static void qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		UnmapViewOfFile(cxt->binary);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	DeleteFile(cxt->obj_file_path);
}
#endif

#ifdef __GNUC__
static void qb_lock_object_file(qb_native_compiler_context *cxt) {
	mprotect(cxt->binary, cxt->binary_size, PROT_EXEC | PROT_READ);
}
#endif

#ifdef _MSC_VER
static void qb_lock_object_file(qb_native_compiler_context *cxt) {
	VirtualProtect(cxt->binary, cxt->binary_size, FILE_MAP_EXECUTE | FILE_MAP_READ, NULL);
}
#endif

static uint32_t qb_attach_symbol(qb_native_compiler_context *cxt, const char *symbol_name, const char *address) {
	uint32_t count = 0;
	if(strncmp(symbol_name, "QBN_", 4) == 0) {
		qb_native_proc proc = (qb_native_proc) address;
		uint64_t crc64 = strtoull(symbol_name + 4, NULL, 16);
		uint32_t i;
		for(i = 0; i < cxt->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
			if(compiler_cxt->compiled_function->instruction_crc64 == crc64) {
				compiler_cxt->compiled_function->native_proc = proc;
				count++;
			}
		}
	} 
	return count;
}

static void qb_detach_symbols(qb_native_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		compiler_cxt->compiled_function->native_proc = NULL;
	}
	cxt->qb_version = 0;
}

#ifdef __ELF__
	#ifdef __LP64__
static int32_t qb_parse_elf64(qb_native_compiler_context *cxt) {
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
			if(!symbol_address || symbol_address == (void *) -1) {
				qb_abort("missing symbol: %s\n", symbol_name);
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
static int32_t qb_parse_elf32(qb_native_compiler_context *cxt) {
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
			if(!symbol_address || symbol_address == (void *) -1) {
				qb_abort("missing symbol: %s\n", symbol_name);
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
static int32_t qb_parse_macho64(qb_native_compiler_context *cxt) {
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
				if(!symbol_address || symbol_address == (void *) -1) {
					qb_abort("missing symbol: %s\n", symbol_name);
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
	uint32_t count = 0;
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
static int32_t qb_parse_macho32(qb_native_compiler_context *cxt) {
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
				if(!symbol_address || symbol_address == (void *) -1) {
					qb_abort("missing symbol: %s\n", symbol_name);
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
static int32_t qb_parse_coff(qb_native_compiler_context *cxt) {
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
					if(!symbol_address || symbol_address == (void *) -1) {
						if(strcmp(symbol_name, "_floor") == 0) {
							symbol_address = floor;
						} else {
							qb_abort("missing symbol: %s\n", symbol_name);
						}
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

void qb_free_native_code(qb_native_code_bundle *bundle) {
#ifdef __GNUC__
	munmap(bundle->memory, bundle->size);
#endif
#ifdef _MSC_VER
	UnmapViewOfFile(bundle->memory);
#endif
}

static void qb_create_cache_folder(qb_native_compiler_context *cxt) {
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
static void qb_link_debuggable_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		for(j = 0; j < native_proc_table_size; j++) {
			qb_native_proc_record *rec = &native_proc_table[j];
			if(compiler_cxt->compiled_function->instruction_crc64 == rec->crc64) {
				compiler_cxt->compiled_function->native_proc = rec->proc;
			}
		}
	}
}
#endif

int32_t qb_decompress_code(qb_native_compiler_context *cxt) {
	// decompress string tables used to generate the C source code
	if(!cxt->pool->op_actions) {
		qb_uncompress_table(compressed_table_native_actions, (void ***) &cxt->pool->op_actions, NULL, 0);
	}
	if(!cxt->pool->op_function_usages) {
		qb_uncompress_table(compressed_table_native_references, (void ***) &cxt->pool->op_function_usages, NULL, 0);
	}
	if(!cxt->pool->function_prototypes) {
		uint32_t count;
		qb_uncompress_table(compressed_table_native_prototypes, (void ***) &cxt->pool->function_prototypes, &count, 0);
#if ZEND_DEBUG
		if(count > PROTOTYPE_COUNT) {
			qb_abort("not enough space for the number of possible prototypes");
		}
#endif
	}

	cxt->op_names = cxt->pool->op_names;
	cxt->op_actions = cxt->pool->op_actions;
	cxt->op_function_usages = cxt->pool->op_function_usages;
	cxt->function_prototypes = cxt->pool->function_prototypes;

	return (cxt->op_actions && cxt->op_function_usages && cxt->function_prototypes);
}

void qb_compile_to_native_code(qb_native_compiler_context *cxt) {
	USE_TSRM
	uint32_t i, attempt;
	int32_t success = FALSE;

#if ZEND_DEBUG
	if(native_proc_table) {
		// link the functions to code in qb_native_proc_debug.c instead of compiling them live
		qb_link_debuggable_functions(cxt);
		return;
	}
#endif

	// make sure the cache folder exists
	qb_create_cache_folder(cxt);

	// XOR the crc64 of the functions together and use it as the file id
	cxt->file_id = 0;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		qb_function *qfunc = compiler_cxt->compiled_function;
		if(!qfunc->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			cxt->file_id ^= qfunc->instruction_crc64;
		}
	}
	spprintf(&cxt->obj_file_path, 0, "%s%cQB%" PRIX64 ".o", cxt->cache_folder_path, PHP_DIR_SEPARATOR, cxt->file_id);

#if ZEND_DEBUG
	for(attempt = 2; attempt <= 2 && !success; attempt++) {
#else
	for(attempt = 1; attempt <= 2 && !success; attempt++) {
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
					success = TRUE;
				} else {
					qb_detach_symbols(cxt);
				}
			}
			if(!success) {
				qb_remove_object_file(cxt);
			}
		}
	} 
}

void qb_initialize_native_compiler_context(qb_native_compiler_context *cxt, qb_build_context *build_cxt TSRMLS_DC) {
	static int hashes_initialized = FALSE;
	if(!hashes_initialized) {
		uint32_t i;
		// calculate hash for faster lookup
		for(i = 0; i < global_native_symbol_count; i++) {
			qb_native_symbol *symbol = &global_native_symbols[i];
			symbol->hash_value = zend_hash_func(symbol->name, strlen(symbol->name) + 1);
		}
		hashes_initialized = TRUE;
	}

	memset(cxt, 0, sizeof(qb_native_compiler_context));

	cxt->pool = build_cxt->pool;
	cxt->print_errors = QB_G(show_compiler_errors);
	cxt->print_source = QB_G(show_native_source);
	cxt->compiler_contexts = build_cxt->compiler_contexts;
	cxt->compiler_context_count = build_cxt->compiler_context_count;
	SAVE_TSRMLS

	cxt->cache_folder_path = QB_G(native_code_cache_path);
}

void qb_free_native_compiler_context(qb_native_compiler_context *cxt) {
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

#else

#endif	// NATIVE_COMPILE_ENABLED
