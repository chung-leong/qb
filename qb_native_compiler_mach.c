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
			args[argc++] = "clang";
		}
		args[argc++] = "-c";
		args[argc++] = "-O2";										// optimization level
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

#ifdef __x86_64__

#pragma pack(push,1)
typedef struct qb_elf_entry {
	int16_t opcode;
	int32_t rip_address;
	int16_t padding;
	void *function_address;
} qb_elf_entry;
#pragma pack(pop)

static void * qb_find_symbol_plt_entry(qb_native_compiler_context *cxt, const char *name) {
	static qb_elf_entry *procedure_linkage_table = NULL;
	uint32_t i, name_len = (uint32_t) strlen(name);
	long hash_value = zend_get_hash_value(name, name_len + 1);
	if(!procedure_linkage_table) {
		procedure_linkage_table = mmap(NULL, sizeof(qb_elf_entry) * global_native_symbol_count, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	}
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				qb_elf_entry *plt_entry = &procedure_linkage_table[i];
				if(!plt_entry->opcode) {
					plt_entry->opcode = 0x25FF;
					plt_entry->rip_address = 2;
					plt_entry->function_address = qb_get_symbol_address(cxt, symbol);
				}
				if(plt_entry->function_address) {
					return plt_entry;
				}
			}
		}
	}
	return NULL;
}
#endif

#ifdef __x86_64__
#define CPU_TYPE_EXPECTED		CPU_TYPE_X86_64
#define ARCH_SELECT(t)			t##_64
#else 
#define	CPU_TYPE_EXPECTED		CPU_TYPE_X86
#define ARCH_SELECT(t)			t
#endif

#define SYMBOL_PREFIX_LENGTH	1

typedef struct ARCH_SELECT(mach_header)			mach_header;
typedef struct ARCH_SELECT(segment_command)		mach_segment_command;
typedef struct ARCH_SELECT(section)				mach_section;
typedef struct ARCH_SELECT(nlist)				mach_nlist;
typedef struct load_command						mach_load_command;
typedef struct symtab_command					mach_symtab_command;
typedef struct relocation_info					mach_relocation_info;

static int32_t qb_parse_object_file(qb_native_compiler_context *cxt, int fd) {
	struct stat stat_buf;
	size_t file_size = 0;

	if(fstat(fd, &stat_buf) != -1) {
		file_size = stat_buf.st_size;
	}
	if(file_size < sizeof(mach_header)) {
		return FALSE;
	}

	mach_header _header, *header = &_header;
	if(read(fd, header, sizeof(mach_header)) == -1) {
		return FALSE;
	}
	if(header->magic != ARCH_SELECT(MH_MAGIC) || header->filetype != MH_OBJECT) {
		return FALSE;
	}
	if(header->cputype != CPU_TYPE_EXPECTED) {
		return FALSE;
	}

	mach_section *sections = NULL;
	uint32_t section_count, section_position;
	mach_nlist *symbols = NULL;
	uint32_t symbol_count;
	char *string_table;
	uint32_t text_section_number = 0;
	uint32_t i, j;
	uint32_t missing_symbol_count = 0;
	mach_load_command _command, *command = &_command;

	uint32_t position = sizeof(mach_header);
	for(i = 0; i < header->ncmds; i++) {
		if(lseek(fd, position, SEEK_SET) == -1 || read(fd, command, sizeof(mach_load_command)) == -1) {
			return FALSE;
		}

		if(command->cmd == ARCH_SELECT(LC_SEGMENT)) {
			// there's only one segment inside a mach-o object file
			// section structures of the segment are located right after the segment command
			mach_segment_command _segment_command, *segment_command = &_segment_command;
			if(lseek(fd, position, SEEK_SET) == -1 || read(fd, segment_command, sizeof(mach_segment_command)) == -1) {
				return FALSE;
			}
			section_count = segment_command->nsects;
			sections = alloca(sizeof(mach_section) * section_count);
			if(read(fd, sections, sizeof(mach_section) * section_count) == -1) {
				return FALSE;
			}
			section_position = position + sizeof(mach_segment_command);
		} else if(command->cmd == LC_SYMTAB) {
			mach_symtab_command _symtab_command, *symtab_command = &_symtab_command;
			if(lseek(fd, position, SEEK_SET) == -1 || read(fd, symtab_command, sizeof(mach_symtab_command)) == -1) {
				return FALSE;
			}
			symbol_count = symtab_command->nsyms;
			symbols = alloca(sizeof(mach_nlist) * symbol_count);
			if(lseek(fd, symtab_command->symoff, SEEK_SET) == -1 || read(fd, symbols, sizeof(mach_nlist) * symbol_count) == -1) {
				return FALSE;
			}
			string_table = alloca(symtab_command->strsize);
			if(lseek(fd, symtab_command->stroff, SEEK_SET) == -1 || read(fd, string_table, symtab_command->strsize) == -1) {
				return FALSE;
			}
		}
		position += command->cmdsize;
	}

	if(!sections || !symbols) {
		return FALSE;
	}

	// align the sections
	uintptr_t address = 0;
	for(i = 0; i < section_count; i++) {
		mach_section *section = &sections[i];
		uint32_t alignment = 1 << section->align;
		section->addr = ALIGN_TO(address, alignment);
		address = section->addr + section->size;
	}

	// allocate memory
	cxt->binary_size = address;
	cxt->binary = mmap(NULL, cxt->binary_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if(cxt->binary == (void *) -1) {
		return FALSE;
	}

	// load the sections
	for(i = 0; i < section_count; i++) {
		mach_section *section = &sections[i];
		if(lseek(fd, section->offset, SEEK_SET) == -1 || read(fd, cxt->binary + section->addr, section->size) == -1) {
			return FALSE;
		}
		if(memcmp(section->sectname, "__text", 7) == 0) {
			text_section_number = i + 1;
		}
	}

	// perform relocation

	for(i = 0; i < section_count; i++) {
		mach_section *section = &sections[i];
		mach_relocation_info *relocations;
		uint32_t relocation_count = section->nreloc;

		if(relocation_count > 0) {
			relocations = alloca(sizeof(mach_relocation_info) * relocation_count);
			if(lseek(fd, section->reloff, SEEK_SET) == -1 || read(fd, relocations, sizeof(mach_relocation_info) * relocation_count) == -1) {
				return FALSE;
			}
		}

#ifdef __LP64__
		intptr_t T = 0;
#endif
		for(j = 0; j < relocation_count; j++) {
			mach_relocation_info *reloc = &relocations[j];

			if(!(reloc->r_address & R_SCATTERED) && reloc->r_extern) {
				void *symbol_address;
				void *target_address = cxt->binary + section->addr + reloc->r_address;
				mach_nlist *symbol = &symbols[reloc->r_symbolnum];
				const char *symbol_name = string_table + symbol->n_un.n_strx;
				if(symbol->n_type == N_EXT) {
#ifdef __x86_64__
					symbol_address = qb_find_symbol_plt_entry(cxt, symbol_name + SYMBOL_PREFIX_LENGTH);
#else
					symbol_address = qb_find_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH);
#endif
					if(!symbol_address) {
						qb_report_missing_native_symbol_exception(0, symbol_name + SYMBOL_PREFIX_LENGTH);
						missing_symbol_count++;
						continue;
					}
				} else {
					mach_nlist *symbol = &symbols[i];
					symbol_address = cxt->binary + symbol->n_value;
				}

				intptr_t S = (intptr_t) symbol_address;
				intptr_t P = ((intptr_t) target_address);

				switch(reloc->r_type) {
#ifdef __LP64__
					case X86_64_RELOC_SIGNED:
					case X86_64_RELOC_BRANCH:
						*((int32_t *) target_address) += S - (P + sizeof(int32_t));
						break;
					case X86_64_RELOC_UNSIGNED:
						*((intptr_t *) target_address) += S - T;
						T = 0;
						break;
					case X86_64_RELOC_SUBTRACTOR:
						T = S;
						break;
#else
					case GENERIC_RELOC_VANILLA:
						if(reloc->r_pcrel) {
							*((int32_t *) target_address) = S - (P + sizeof(int32_t));
						} else {
							*((intptr_t *) target_address) = S;
						}
						break;
#endif
					default:
						return FALSE;
				}
			}
		}
	}
	mprotect(cxt->binary, cxt->binary_size, PROT_EXEC | PROT_READ);
	if(missing_symbol_count > 0) {
		return FALSE;
	}

	// find the compiled functions
	uint32_t count = 0;
	for(i = 0; i < symbol_count; i++) {
		mach_nlist *symbol = &symbols[i];
		if(symbol->n_sect != NO_SECT) {
			const char *symbol_name = string_table + symbol->n_un.n_strx;
			void *symbol_address = cxt->binary + symbol->n_value;
			if(symbol->n_sect == text_section_number) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH, symbol_address);
				if(attached == 0) {
					if(!qb_check_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH)) {
						return FALSE;
					}
				}
				count += attached;
			} else {
				if(strncmp(symbol_name + SYMBOL_PREFIX_LENGTH, "QB_VERSION", 10) == 0) {
					uint32_t *p_version = symbol_address;
					cxt->qb_version = *p_version;
				}
			}
		}
	}
	return (count > 0);
}

static int32_t qb_load_object_file(qb_native_compiler_context *cxt) {
	// map the file into memory
	int fd = open(cxt->obj_file_path, O_RDONLY);
	int32_t result;
	if(fd == -1) {
		return FALSE;
	}
	result = qb_parse_object_file(cxt, fd);
	close(fd);
	return result;
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

static void * qb_get_intrinsic_function_address(const char *name) {
	void *address = NULL;
#ifdef HAVE_SINCOS
	if(!address) {
		if(strcmp(name, "sincos") == 0) {
			address = sincos;
		} else if(strcmp(name, "sincosf") == 0) {
			address = sincosf;
		}
	}
#endif
	return address;
}
