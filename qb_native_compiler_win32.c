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

static void qb_create_cache_folder(qb_native_compiler_context *cxt) {
	uint32_t len = (uint32_t) strlen(cxt->cache_folder_path);
	if(len == 0) {
		// use the system temp folder when no cache folder is specified
		len = GetTempPath(0, NULL);
		cxt->cache_folder_path = emalloc(len + 1);
		GetTempPath(len + 1, cxt->cache_folder_path);
		cxt->cache_folder_path[len - 1] = '\0';
	}
	CreateDirectory(cxt->cache_folder_path, NULL);
}

static int32_t qb_launch_compiler(qb_native_compiler_context *cxt) {
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
	int file_descriptor;
#ifndef  _WIN64
	int cpu_flags[4];
#endif
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

#ifdef  _WIN64
	// SSE2 is always available in x64 
	sse_option = "";
#else
	// see if the CPU supports SSE
	__cpuid(cpu_flags, 0x00000001);
	if(cpu_flags[2] & (1 << 20) || cpu_flags[2] & (1 << 19) || cpu_flags[2] & (1 << 0) || cpu_flags[3] & (1 << 26)) {
		sse_option = "/arch:SSE2";
	} else if(cpu_flags[3] & (1 << 25)) {
		sse_option = "/arch:SSE";
	} else {
		sse_option = "";
	}
#endif

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
	spprintf(&command_line, 0, "\"%s\" /O2 /Oy /GS- /fp:precise %s /nologo /Fo\"%s\" /c \"%s\"", compiler_path, sse_option, cxt->obj_file_path, cxt->c_file_path);
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

static int32_t qb_wait_for_compiler_response(qb_native_compiler_context *cxt) {
	USE_TSRM
	char buffer[256];
	size_t count;
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
			php_write(buffer, (uint32_t) count TSRMLS_CC);
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

#ifdef _WIN64	
#define IMAGE_FILE_MACHINE		IMAGE_FILE_MACHINE_AMD64
#define SYMBOL_PREFIX_LENGTH	0
#else
#define IMAGE_FILE_MACHINE		IMAGE_FILE_MACHINE_I386
#define SYMBOL_PREFIX_LENGTH	1
#endif

#ifdef _WIN64
static void * qb_find_symbol_pointer(qb_native_compiler_context *cxt, const char *name) {
	static void **import_address_table = NULL;
	uint32_t i, name_len = (uint32_t) strlen(name);
	long hash_value = zend_get_hash_value(name, name_len + 1);
	if(!import_address_table) {
		import_address_table = VirtualAlloc(NULL, sizeof(void *) * global_native_symbol_count, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				void *address = qb_get_symbol_address(cxt, symbol);
				if(address) {
					import_address_table[i] = address;
					return &import_address_table[i];
				} else {
					break;
				}
			}
		}
	}
	return NULL;
}
#endif

static int32_t qb_parse_object_file(qb_native_compiler_context *cxt, HANDLE file) {
	IMAGE_FILE_HEADER _header, *header = &_header;
	IMAGE_SECTION_HEADER *sections;
	IMAGE_SYMBOL *symbols;
	DWORD file_size, bytes_read;
	DWORD symbol_table_size;
	uint32_t address;
	uint32_t i, j;
	char *string_section;
	uint32_t count = 0;
	uint32_t missing_symbol_count = 0;
	
	file_size = GetFileSize(file, NULL);
	if(file_size < sizeof(IMAGE_FILE_HEADER)) {
		return FALSE;
	}
	if(!ReadFile(file, header, sizeof(IMAGE_FILE_HEADER), &bytes_read, NULL)) {
		return FALSE;
	}
	if(header->Machine != IMAGE_FILE_MACHINE) {
		return FALSE;
	}
	sections = alloca(sizeof(IMAGE_SECTION_HEADER) * header->NumberOfSections);

	if(!ReadFile(file, sections, sizeof(IMAGE_SECTION_HEADER) * header->NumberOfSections, &bytes_read, NULL)) {
		return FALSE;
	}

	// make sure sections are aligned
	address = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER) * header->NumberOfSections;
	for(i = 0; i < header->NumberOfSections; i++) {
		IMAGE_SECTION_HEADER *section = &sections[i];
		uint32_t alignment = 1 << (((section->Characteristics & IMAGE_SCN_ALIGN_MASK) >> 20) - 1);
		uint32_t section_size = section->SizeOfRawData + sizeof(IMAGE_RELOCATION) * section->NumberOfRelocations; 
		section->VirtualAddress = ALIGN_TO(address, alignment);
		section->Misc.VirtualSize = section_size;
		address = section->VirtualAddress + section_size;
	}

	// allocate memory
	cxt->binary_size = address;
	cxt->binary = VirtualAlloc(NULL, cxt->binary_size,  MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!cxt->binary) {
		return FALSE;
	}

	// load the sections
	for(i = 0; i < header->NumberOfSections; i++) {
		IMAGE_SECTION_HEADER *section = &sections[i];
		uint32_t section_size = section->Misc.VirtualSize;
		int32_t offset;
		
		if(SetFilePointer(file, section->PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			return FALSE;
		}
		if(!ReadFile(file, cxt->binary + section->VirtualAddress, section_size, &bytes_read, NULL)) {
			return FALSE;
		}
		offset = section->VirtualAddress - section->PointerToRawData;
		section->PointerToRawData += offset;
		if(section->NumberOfRelocations > 0) {
			section->PointerToRelocations += offset;
		}
		if(section->NumberOfLinenumbers > 0) {
			section->NumberOfLinenumbers +=  offset;
		}
	}
	memcpy(cxt->binary, header, sizeof(IMAGE_FILE_HEADER));
	memcpy(cxt->binary + sizeof(IMAGE_FILE_HEADER), sections, sizeof(IMAGE_SECTION_HEADER) * header->NumberOfSections);

	// load the symbols
	symbol_table_size = file_size - header->PointerToSymbolTable;
	symbols = alloca(symbol_table_size);
	string_section = (char *) &symbols[header->NumberOfSymbols];
	if(SetFilePointer(file, header->PointerToSymbolTable, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		return FALSE;
	}
	if(!ReadFile(file, symbols, symbol_table_size, &bytes_read, NULL)) {
		return FALSE;
	}

	// perform relocation
	for(i = 0; i < header->NumberOfSections; i++) {
		IMAGE_SECTION_HEADER *section = &sections[i];
		if(section->Characteristics & IMAGE_SCN_CNT_CODE) {
			IMAGE_RELOCATION *relocations = (IMAGE_RELOCATION *) (cxt->binary + section->PointerToRelocations);
			uint32_t relocation_count = section->NumberOfRelocations;

			for(j = 0; j < relocation_count; j++) {
				IMAGE_RELOCATION *reloc = &relocations[j];
				IMAGE_SYMBOL *symbol = &symbols[reloc->SymbolTableIndex];
				char *symbol_name = (symbol->N.Name.Short) ? symbol->N.ShortName : string_section + symbol->N.Name.Long;
				void *symbol_address;
				void *target_address = cxt->binary + section->VirtualAddress + reloc->VirtualAddress;
				intptr_t S, P, B = (intptr_t) cxt->binary; 

				if(symbol->SectionNumber == IMAGE_SYM_UNDEFINED) {
#ifdef _WIN64
					if(!ISFCN(symbol->Type) && memcmp(symbol_name, "__imp_", 6) == 0) {
						if(memcmp(symbol_name, "__imp_intrinsic_", 16) == 0) {
							symbol_address = qb_find_symbol_pointer(cxt, symbol_name + 16);
						} else {
							symbol_address = qb_find_symbol_pointer(cxt, symbol_name + 6);
						}
					} else {
						symbol_address = qb_find_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH);
					}
#else
					symbol_address = qb_find_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH);
#endif
					if(!symbol_address) {
						if(strcmp(symbol_name, "__ImageBase") == 0) {
							symbol_address = cxt->binary;
						} else {
							qb_report_missing_native_symbol_exception(NULL, 0, symbol_name + SYMBOL_PREFIX_LENGTH);
							missing_symbol_count++;
							continue;
						}
					}
				} else {
					// probably something in the data segment (e.g. a string literal)
					IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
					symbol_address = cxt->binary + section->VirtualAddress + symbol->Value;
				}

				S = (intptr_t) symbol_address;
				P = (intptr_t) target_address;

				switch(reloc->Type) {
#ifdef _WIN64				
					case IMAGE_REL_AMD64_ABSOLUTE:
						*((intptr_t *) target_address) = S;
						break;
					case IMAGE_REL_AMD64_REL32:
						*((int32_t *) target_address) += (int32_t) (S - (P + sizeof(int32_t)));
						break;
					case IMAGE_REL_AMD64_ADDR32NB:
						*((int32_t *) target_address) += (int32_t) (S - B);
						break;
#else				
					case IMAGE_REL_I386_DIR32:
						*((intptr_t *) target_address) += S;
						break;
					case IMAGE_REL_I386_REL32:
						*((intptr_t *) target_address) += (int32_t) (S - (P + sizeof(intptr_t)));
						break;
#endif						
					default:
						return FALSE;
				}
			}
		}
	}
	VirtualProtect(cxt->binary, cxt->binary_size, FILE_MAP_EXECUTE | FILE_MAP_READ, NULL);

	if(missing_symbol_count > 0) {
		return FALSE;
	}

	// find the compiled functions
	for(i = 0; i < header->NumberOfSymbols; i++) {
		IMAGE_SYMBOL *symbol = &symbols[i];
		char *symbol_name = (symbol->N.Name.Short) ? symbol->N.ShortName : (string_section + symbol->N.Name.Long);
		if(symbol->SectionNumber > 0) {
			IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
			void *symbol_address = cxt->binary + section->VirtualAddress + symbol->Value;
			if(ISFCN(symbol->Type)) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH, symbol_address);
				if(!attached) {
					// error out if there's an unrecognized function
					if(!qb_check_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH)) {
						return FALSE;
					}
				}
				count += attached;
			} else if(symbol->StorageClass == IMAGE_SYM_CLASS_EXTERNAL) {
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
	HANDLE file = NULL;
	int32_t result;

	file = CreateFile(cxt->obj_file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if(file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	result = qb_parse_object_file(cxt, file);
	CloseHandle(file);
	return result;
}

static void qb_remove_object_file(qb_native_compiler_context *cxt) {
	DeleteFile(cxt->obj_file_path);
}

void qb_free_native_code(qb_native_code_bundle *bundle) {
	VirtualFree(bundle->memory, 0, MEM_RELEASE); 
}

static void * qb_get_intrinsic_function_address(const char *name) {
	void *address = NULL;
#ifdef _WIN64

#else
	if(strcmp(name, "floor") == 0) {
		address = floor;
	} else if(strcmp(name, "_ftol2") == 0) {
		address = _ftol2;
	} else if(strcmp(name, "_ftol2_sse") == 0) {
		address = _ftol2_sse;
	} else if(strcmp(name, "_allshr") == 0) {
		address = _allshr;
	} else if(strcmp(name, "_allshl") == 0) {
		address = _allshl;
	}
#endif
	if(!address) {
		// intrinsics employed by MSVC are located in the runtime DLL
		// since the QB DLL might be used compiled using the same version of MSVC
		// we'll look for the function dynamically
#ifdef ZEND_DEBUG
		static const char *dll_names[] = { "msvcr110.dll", "msvcr110d.dll", "msvcr100.dll", "msvcr100d.dll", "msvcr90.dll", "msvcr90d.dll" };
#else
		static const char *dll_names[] = { "msvcr110.dll", "msvcr100.dll", "msvcr90.dll" };
#endif
		static HMODULE c_lib_handle = 0;
		static const char *c_lib_dll_name = NULL;

		if(!c_lib_handle) {
			// see which of these DLL's is loaded
			uint32_t i;
			for(i = 0; i < sizeof(dll_names) / sizeof(dll_names[0]); i++) {
				c_lib_handle = GetModuleHandle(dll_names[i]);
				if(c_lib_handle) {
					c_lib_dll_name = dll_names[i];
					break;
				}
			}
		}
		if(c_lib_handle) {
			address = GetProcAddress(c_lib_handle, name);
		}
		if(!address) {
			// try loading a more recent version of the runtime
			static HMODULE newer_c_lib_handle = 0;
			if(!newer_c_lib_handle) {
				uint32_t i;
				for(i = 0; i < sizeof(dll_names) / sizeof(dll_names[0]); i++) {
					if(dll_names[i] != c_lib_dll_name) {
						newer_c_lib_handle = LoadLibrary(dll_names[i]);
						if(newer_c_lib_handle) {
							break;
						}
					}
				}
			}
			if(newer_c_lib_handle) {
				address = GetProcAddress(newer_c_lib_handle, name);
			}
		}
	}
	return address;
}
