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

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <mach-o/loader.h>
#include <mach-o/reloc.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/x86_64/reloc.h>

static void qb_create_cache_folder(qb_native_compiler_context *cxt) {
	uint32_t len = (uint32_t) strlen(cxt->cache_folder_path);
	if(len == 0) {
		// use the system temp folder when no cache folder is specified
		char *temp_folder = getenv("TMPDIR");
		if(!temp_folder) {
			temp_folder = "/tmp";
		}
		len = strlen(temp_folder);
		if(temp_folder[len - 1] == '/') {
			len--;
		}
		cxt->cache_folder_path = estrndup(temp_folder, len);
	}
	mkdir(cxt->cache_folder_path, 0777);
}

static int32_t qb_launch_compiler(qb_native_compiler_context *cxt) {
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
		args[argc++] = "-o";
		args[argc++] = cxt->obj_file_path;
		args[argc++] = "-xc";										// indicate the source is C
		args[argc++] = "-";											// input from stdin
		args[argc++] = NULL;

		if(strlen(compiler_env_path) > 0) {
			setenv("PATH", compiler_env_path, TRUE);
		}
		execvp(args[0], (char **) args);
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
			binary = mmap((void *) ~0, binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, file_descriptor, 0);
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

#ifdef __x86_64__
#define CPU_TYPE_EXPECTED		CPU_TYPE_X86_64
#define ARCH_SELECT(t)			token##_64
#else 
#define	CPU_TYPE_EXPECTED		CPU_TYPE_X86
#define ARCH_SELECT(t)			token
#endif

typedef struct ARCH_SELECT(mach_header)			mach_header;
typedef struct ARCH_SELECT(segment_command)		segment_command;
typedef struct ARCH_SELECT(section)				section;
typedef struct ARCH_SELECT(nlist)				nlist;
typedef struct load_command						load_command;
typedef struct symtab_command					symtab_command;
typedef struct relocation_info					relocation_info

static int32_t qb_parse_mach_o(qb_native_compiler_context *cxt) {
	mach_header *header;
	load_command *command;
	segment_command_64 *segment_command;
	section *sections = NULL;
	symtab_command *symtab_command;
	nlist *symbols = NULL;
	char *string_table;
	relocation_info *relocations;
	uint32_t relocation_count;
	char *text_section = NULL;
	uint32_t text_section_number = 0;
	uint32_t i;
	uint32_t missing_symbol_count = 0;

	if(cxt->binary_size < sizeof(mach_header)) {
		return FALSE;
	}

	header = (mach_header *) cxt->binary;
	if(header->magic != ARCH_SELECT(MH_MAGIC) || header->filetype != MH_OBJECT) {
		return FALSE;
	}
	if(header->cputype != CPU_TYPE_EXPECTED) {
		return FALSE;
	}

	command = (load_command *) (cxt->binary + sizeof(mach_header));
	for(i = 0; i < header->ncmds; i++) {
		if(command->cmd == ARCH_SELECT(LC_SEGMENT)) {
			// there's only one segment inside a mach-o object file
			// section structures of the segment are located right after the segment command
			segment_command = (segment_command *) command;
			sections = (section *) ((char *) command + sizeof(segment_command_64));
		} else if(command->cmd == LC_SYMTAB) {
			symtab_command = (symtab_command *) command;
			symbols = (nlist *) (cxt->binary + symtab_command->symoff);
			string_table = cxt->binary + symtab_command->stroff;
		}
		command = (load_command *) (((char *) command) + command->cmdsize);
	}

	if(!sections || !symbols) {
		return FALSE;
	}

	for(i = 0; i < segment_command->nsects; i++) {
		section *section = &sections[i];
		if(memcmp(section->sectname, "__text", 7) == 0) {
			relocations = (relocation_info *) (cxt->binary + section->reloff);
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
		relocation_info *reloc = &relocations[i];

		if(!(reloc->r_address & R_SCATTERED)) {
			void *symbol_address;
			void *target_address = text_section + reloc->r_address;
			if(reloc->r_extern) {
				nlist *symbol = &symbols[reloc->r_symbolnum];
				const char *symbol_name = string_table + symbol->n_un.n_strx;
				symbol_address = qb_find_symbol(cxt, symbol_name, TRUE);
				if(!symbol_address) {
					qb_report_missing_native_symbol_exception(NULL, 0, symbol_name);
					missing_symbol_count++;
					continue;
				}
			} else {
				section *section = &sections[reloc->r_symbolnum - 1];
				symbol_address = cxt->binary + section->offset;
			}

			intptr_t S = (intptr_t) symbol_address;
			intptr_t P = ((intptr_t) target_address);

			switch(reloc->r_type) {
				case GENERIC_RELOC_VANILLA: 
				case X86_64_RELOC_BRANCH: {
					if(reloc->r_pcrel) {
						*((int32_t *) target_address) = S - (P + sizeof(intptr_t));
					} else {
						*((intptr_t *) target_address) = S;
					}
				}	break;
				default:
					return FALSE;
			}
		}
	}
	if(missing_symbol_count > 0) {
		return FALSE;
	}

	// find the compiled functions
	uint32_t count = 0;
	for(i = 0; i < symtab_command->nsyms; i++) {
		nlist *symbol = &symbols[i];
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

static void qb_lock_object_file(qb_native_compiler_context *cxt) {
	mprotect(cxt->binary, cxt->binary_size, PROT_EXEC | PROT_READ);
}

static void qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		munmap(cxt->binary, cxt->binary_size);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	unlink(cxt->obj_file_path);
}

void qb_free_native_code(qb_native_code_bundle *bundle) {
	munmap(bundle->memory, bundle->size);
}
