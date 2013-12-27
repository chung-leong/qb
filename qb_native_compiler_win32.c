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

static int32_t qb_load_object_file(qb_native_compiler_context *cxt) {
	USE_TSRM
	HANDLE file = NULL, mapping = NULL;

	file = CreateFile(cxt->obj_file_path, GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if(file != INVALID_HANDLE_VALUE) {
		mapping = CreateFileMapping(file, NULL, PAGE_EXECUTE_WRITECOPY, 0, 0, NULL);
		if(mapping != INVALID_HANDLE_VALUE) {
#ifdef _WIN64
			// load the object at an address within 4G of the DLL's base address
			char *base_address = (char *) 0x00007FFF00000000;
			do {
				base_address += 0x100000;
				cxt->binary = MapViewOfFileEx(mapping, FILE_MAP_EXECUTE | FILE_MAP_COPY, 0, 0, 0, base_address);
			} while(!cxt->binary && base_address < (char *) 0x800000000000);
#else
			cxt->binary = MapViewOfFileEx(mapping, FILE_MAP_EXECUTE | FILE_MAP_COPY, 0, 0, 0, NULL);
#endif
			cxt->binary_size = GetFileSize(file, NULL);
		}
	}

	CloseHandle(mapping);
	CloseHandle(file);		
	return (cxt->binary != NULL);
}

#ifdef _WIN64	
#define IMAGE_FILE_MACHINE		IMAGE_FILE_MACHINE_AMD64
#define SYMBOL_PREFIX_LENGTH	0
#else
#define IMAGE_FILE_MACHINE		IMAGE_FILE_MACHINE_I386
#define SYMBOL_PREFIX_LENGTH	1
#endif

static int32_t qb_parse_object_file(qb_native_compiler_context *cxt) {
	IMAGE_FILE_HEADER *header;
	IMAGE_SECTION_HEADER *sections;
	IMAGE_SYMBOL *symbols;
	char *string_section;
	uint32_t count = 0;
	uint32_t i, j;
	uint32_t missing_symbol_count = 0;
	
	if(cxt->binary_size < sizeof(IMAGE_FILE_HEADER)) {
		return FALSE;
	}

	header = (IMAGE_FILE_HEADER *) cxt->binary;
	if(header->Machine != IMAGE_FILE_MACHINE) {
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
				intptr_t S, P; 

				if(symbol->SectionNumber == IMAGE_SYM_UNDEFINED) {
					symbol_address = qb_find_symbol(cxt, symbol_name, TRUE);
					if(!symbol_address) {
						qb_report_missing_native_symbol_exception(NULL, 0, symbol_name);
						missing_symbol_count++;
						continue;
					}
				} else {
					// probably something in the data segment (e.g. a string literal)
					IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
					symbol_address = cxt->binary + section->PointerToRawData + symbol->Value;
				}

				S = (intptr_t) symbol_address;
				P = (intptr_t) target_address;

				switch(reloc->Type) {
#ifdef _WIN64				
					case IMAGE_REL_AMD64_ABSOLUTE:
						*((intptr_t *) target_address) = S;
						break;
					case IMAGE_REL_AMD64_REL32:
						*((int32_t *) target_address) += S - (P + sizeof(int32_t));
						break;
#else				
					case IMAGE_REL_I386_DIR32:
						*((intptr_t *) target_address) += S;
						break;
					case IMAGE_REL_I386_REL32:
						*((intptr_t *) target_address) += S - (P + sizeof(intptr_t));
						break;
#endif						
					default:
						return FALSE;
				}
			}
		}
	}
	if(missing_symbol_count > 0) {
		return FALSE;
	}

	// find the compiled functions
	for(i = 0; i < header->NumberOfSymbols; i++) {
		IMAGE_SYMBOL *symbol = &symbols[i];
		if(symbol->SectionNumber > 0) {
			IMAGE_SECTION_HEADER *section = &sections[symbol->SectionNumber - 1];
			char *symbol_name = (symbol->N.Name.Short) ? symbol->N.ShortName : (string_section + symbol->N.Name.Long);
			void *symbol_address = cxt->binary + section->PointerToRawData + symbol->Value;
			if(ISFCN(symbol->Type)) {
				uint32_t attached = qb_attach_symbol(cxt, symbol_name + SYMBOL_PREFIX_LENGTH, symbol_address);
				if(!attached) {
					// error out if there's an unrecognized function
					if(!qb_find_symbol(cxt, symbol_name, FALSE)) {
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

static void qb_lock_object_file(qb_native_compiler_context *cxt) {
	VirtualProtect(cxt->binary, cxt->binary_size, FILE_MAP_EXECUTE | FILE_MAP_READ, NULL);
}

static void qb_remove_object_file(qb_native_compiler_context *cxt) {
	if(cxt->binary) {
		UnmapViewOfFile(cxt->binary);
		cxt->binary = NULL;
		cxt->binary_size = 0;
	}
	DeleteFile(cxt->obj_file_path);
}

void qb_free_native_code(qb_native_code_bundle *bundle) {
	UnmapViewOfFile(bundle->memory);
}
