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

#include <elf.h>

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
		const char *args[32];
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
		args[argc++] = "-Wno-pointer-sign";
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

static int32_t qb_check_symbol_strip_trailing_tag(qb_native_compiler_context *cxt, const char *name) {
	// icc creates extra symbols ending in ..0, ..1, etc.
	// don't know what they are
	uint32_t name_len = 0;
	while(name[name_len] != '\0' && name[name_len] != '.') {
		name_len++;
	}
	char *new_name = alloca(name_len + 1);
	memcpy(new_name, name, name_len + 1);
	new_name[name_len] = '\0';
	return qb_check_symbol(cxt, new_name);
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

#ifdef __LP64__
#define EM_EXPECTED						EM_X86_64
#define ELF_SIGNATURE_GCC				"\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define ELF_SIGNATURE_ICC				"\x7f\x45\x4c\x46\x02\x01\x01\x03\x00\x00\x00\x00\x00\x00\x00\x00"
#define ELF_R_TYPE(r)					ELF64_R_TYPE(r)
#define ELF_R_SYM(r)					ELF64_R_SYM(r)
#define ELF_ST_BIND(r)					ELF64_ST_BIND(r)
#define ELF_ST_TYPE(s)					ELF64_ST_TYPE(s)
#define SHT_REL_EXPECTED				SHT_RELA
typedef Elf64_Ehdr				Elf_Ehdr;
typedef Elf64_Shdr				Elf_Shdr;
typedef Elf64_Rela				Elf_Rel;
typedef Elf64_Sym				Elf_Sym;
#else
#define EM_EXPECTED						EM_386
#define ELF_SIGNATURE_GCC				"\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define ELF_SIGNATURE_ICC				"\x7f\x45\x4c\x46\x01\x01\x01\x03\x00\x00\x00\x00\x00\x00\x00\x00"
#define ELF_R_TYPE(r)					ELF32_R_TYPE(r)
#define ELF_R_SYM(r)					ELF32_R_SYM(r)
#define ELF_ST_BIND(r)					ELF32_ST_BIND(r)
#define ELF_ST_TYPE(s)					ELF32_ST_TYPE(s)
#define SHT_REL_EXPECTED				SHT_REL
typedef Elf32_Ehdr				Elf_Ehdr;
typedef Elf32_Shdr				Elf_Shdr;
typedef Elf32_Rel				Elf_Rel;
typedef Elf32_Sym				Elf_Sym;
#endif

static int32_t qb_parse_object_file(qb_native_compiler_context *cxt) {
	int32_t icc = FALSE;
	uint32_t missing_symbol_count = 0;

	if(cxt->binary_size < sizeof(Elf_Ehdr)) {
		return FALSE;
	}

	// see if it's has the ELF signature (the fifth byte indicates whether it's 32 or 64 bit)
	Elf_Ehdr *header = (Elf_Ehdr *) cxt->binary;
	if(memcmp(header->e_ident, ELF_SIGNATURE_GCC, 16) != 0) {
		// icc sets the ABI (8th) byte to 0x03
		if(memcmp(header->e_ident, ELF_SIGNATURE_ICC, 16) != 0) {
			return FALSE;
		} else {
			icc = TRUE;
		}
	}

	if(header->e_machine != EM_EXPECTED) {
		return FALSE;
	}

	// check the size
	int section_header_end = header->e_shoff + header->e_shentsize * header->e_shnum;
	if(section_header_end > cxt->binary_size) {
		return FALSE;
	}

	int i, j;
	int section_count = header->e_shnum;
	Elf_Shdr *section_headers = (Elf_Shdr *) (cxt->binary + header->e_shoff);
	Elf_Shdr *section_string_section_header = &section_headers[header->e_shstrndx];
	char *section_string_section = (char *) (cxt->binary + section_string_section_header->sh_offset);

	// look for relocation sections
	for(i = 0; i < section_count; i++) {
		if(section_headers[i].sh_type == SHT_REL_EXPECTED) {
			Elf_Shdr *reloc_section_header = &section_headers[i];
			Elf_Shdr *text_section_header = &section_headers[reloc_section_header->sh_info];
			Elf_Shdr *symbol_section_header = &section_headers[reloc_section_header->sh_link];
			Elf_Shdr *string_section_header = &section_headers[symbol_section_header->sh_link];
			Elf_Rel *relocations = (Elf_Rel *) (cxt->binary + reloc_section_header->sh_offset);
			uint32_t relocation_count = (uint32_t) (reloc_section_header->sh_size / sizeof(Elf_Rel));
			char *text_section = (char *) (cxt->binary + text_section_header->sh_offset);
			char *string_section = (char *) (cxt->binary + string_section_header->sh_offset);
			Elf_Sym *symbols = (Elf_Sym *) (cxt->binary + symbol_section_header->sh_offset);

			for(j = 0; j < relocation_count; j++) {
				Elf_Rel *relocation = &relocations[j];
				int reloc_type = ELF_R_TYPE(relocation->r_info);
				int symbol_index = ELF_R_SYM(relocation->r_info);
				Elf_Sym *symbol = &symbols[symbol_index];
				int symbol_bind = ELF_ST_BIND(symbol->st_info);
				char *symbol_name = string_section + symbol->st_name;
				void *symbol_address;
				void *target_address = (text_section + relocation->r_offset);

				if(symbol_bind == STB_LOCAL) {
					// links to something in another section
					symbol_address = (cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value);
				} else if(symbol_bind == STB_GLOBAL) {
					// links to a symbol symbol
					symbol_address = qb_find_symbol(cxt, symbol_name);
					if(!symbol_address) {
						qb_report_missing_native_symbol_exception(NULL, 0, symbol_name);
						missing_symbol_count++;
						continue;
					}
				} else {
					return FALSE;
				}

				intptr_t P = (intptr_t) target_address;
				intptr_t S = (intptr_t) symbol_address;
#ifdef __LP64__
				intptr_t A = relocation->r_addend;
#endif

				switch(reloc_type) {
#ifdef __LP64__
					case R_X86_64_NONE:
						break;
					case R_X86_64_64:
						*((intptr_t *) target_address) = A + S;
						break;
					case R_X86_64_PC32:
						*((int32_t *) target_address) = A + S - P;
						break;
					case R_X86_64_32:
						*((uint32_t *) target_address) = A + S;
						break;
					case R_X86_64_32S:
						*((int32_t *) target_address) = A + S;
						break;
#else
					case R_386_NONE:
						break;
					case R_386_32:
						*((intptr_t *) target_address) += S;
						break;
					case R_386_PC32:
						*((intptr_t *) target_address) += S - P;
						break;
#endif
					default:
						return FALSE;
				}
			}
		}
	}

	// look for symbol section
	uint32_t count = 0;
	for(i = 0; i < section_count; i++) {
		if(section_headers[i].sh_type == SHT_SYMTAB) {
			Elf_Shdr *symbol_section_header = &section_headers[i];
			char *symbol_section_name = section_string_section + symbol_section_header->sh_name;
			if(strcmp(symbol_section_name, ".symtab") == 0) {
				Elf_Shdr *string_section_header = &section_headers[symbol_section_header->sh_link];
				char *string_section = (char *) (cxt->binary + string_section_header->sh_offset);
				Elf_Sym *symbols = (Elf_Sym *) (cxt->binary + symbol_section_header->sh_offset);
				uint32_t symbol_count = (uint32_t) (symbol_section_header->sh_size / sizeof(Elf_Sym));

				for(i = 0; i < symbol_count; i++) {
					Elf_Sym *symbol = &symbols[i];
					if(symbol->st_shndx < symbol_count) {
						int symbol_type = ELF_ST_TYPE(symbol->st_info);
						char *symbol_name = string_section + symbol->st_name;
						void *symbol_address = cxt->binary + section_headers[symbol->st_shndx].sh_offset + symbol->st_value;
						if(symbol_type == STT_FUNC) {
							uint32_t attached = qb_attach_symbol(cxt, symbol_name, symbol_address);
							if(!attached) {
								// error out if there's an unrecognized function
								if(!qb_check_symbol(cxt, symbol_name)) {
									if(icc) {
										if(!qb_check_symbol_strip_trailing_tag(cxt, symbol_name)) {
											return FALSE;
										}
									} else {
										return FALSE;
									}
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
