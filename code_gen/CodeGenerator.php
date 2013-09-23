<?php

class CodeGenerator {
	protected $compiler;
	protected $handlers = array();
	protected $elementTypes = array("S32", "U32", "S08", "U08", "S16", "U16", "S64", "U64", "F32", "F64");
	protected $floatTypes = array("F32", "F64");
	protected $addressModes = array("SCA", "ELE", "ARR");
	protected $scalarAddressModes = array("SCA", "ELE");
	protected $arrayAddressModes = array("ARR");
	
	protected $currentIndentationLevel;
	
	public function __construct() {
		if(!$this->handlers) {
			$this->addHandlers();
		}
	}
	
	public function writeTypeDeclarations($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;
	
		fwrite($handle, "#pragma pack(push,1)\n\n");
		$structs = $this->getInstructionStructureDefinitions();
		foreach($structs as $struct) {
			$this->writeCode($handle, $struct);
			fwrite($handle, "\n");
		}
		fwrite($handle, "#pragma pack(pop)\n\n");

		$lines = array();
		$lines[] = "extern void *op_handlers[];";
		$lines[] = "";
		$this->writeCode($handle, $lines);
	}
	
	public function writeFunctionPrototypes($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;

		$functions = $this->getFunctionDefinitions();
		$inlinePattern = '/\b(inline|zend_always_inline)\b/';
		
		// add non-inline functions first
		foreach($functions as $function) {
			$line1 = $function[0];
			
			if(!preg_match($inlinePattern, $line1)) {
				$prototype = preg_replace('/\s*{\s*$/', ';', $line1);
				fwrite($handle, "$prototype\n");
			}
		}
		fwrite($handle, "\n");
		
		// add inline-functions
		foreach($functions as $function) {
			$line1 = $function[0];
			
			if(preg_match($inlinePattern, $line1)) {
				$this->writeCode($handle, $function);
				fwrite($handle, "\n");
			}
		}
	}

	public function writeFunctionDefinitions($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;
		
		$functions = $this->getFunctionDefinitions();
		foreach($functions as $function) {
			$line1 = $function[0];
		
			if(!preg_match('/\b(inline|zend_always_inline)\b/', $line1)) {
				$this->writeCode($handle, $function);
				fwrite($handle, "\n");
			}
		}
	}
	
	public function writeMainLoop($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;
		
		$lines = array();
		$lines[] = "";			
		$lines[] = "void ZEND_FASTCALL qb_main(qb_interpreter_context *__restrict cxt) {";
		$lines[] =		"";
		$lines[] = 		"if(cxt) {";
		$lines[] = 			"register void *__restrict handler;";
		$lines[] = 			"register int8_t *__restrict ip;";
		
		if($compiler == "MSVC") {
			$lines[] =		"uint32_t windows_timeout_check_counter = 0;";
			$lines[] = 		"volatile zend_bool *windows_timed_out_pointer = cxt->windows_timed_out_pointer;";
		}
		$lines[] =			"USE_TSRM";
		$lines[] = 			"";
		$lines[] = 			"{";
		$lines[] = 				"ip = cxt->function->instructions;";
		$lines[] = 				"handler = *((void **) ip);";
		$lines[] = 				"ip += sizeof(void *);";
		$lines[] =			"";
		if($compiler == "GCC") {
			$lines[] = 			"goto *handler;";
		}
		$lines[] = 			"}";
		$lines[] = "";
		if($compiler == "MSVC") {
			// Visual C doesn't support computed goto so we have to use a giant switch() statement instead
			$lines[] = 		"do {";
			$lines[] = 			"switch((int) handler) {";
		}
		$this->writeCode($handle, $lines);
		
		foreach($this->handlers as $handler) {
			$lines = $handler->getCode();
			$lines[] = "";
			$this->writeCode($handle, $lines);
		}
		
		$lines = array();
		if($compiler == "MSVC") {
			$lines[] = 			"default:";
			$lines[] = 				"__assume(0);";
			$lines[] = 			"}";
			$lines[] = 		"} while(1);";
		}
		if($compiler == "GCC") {
			$lines[] = 	"} else {";
			foreach($this->handlers as $handler) {
				$name = $handler->getName();
				$lines[] = 		"op_handlers[QB_$name] = &&label_$name;";
			}
		}
		$lines[] = 		"}";
		$this->writeCode($handle, $lines);

		$lines = array();
		$lines[] =		"";
		$lines[] = 	"}";
		$lines[] = 	"";
		if($compiler == "GCC") {
			$lines[] = 	"void *op_handlers[QB_OPCODE_COUNT];";
		}
		$this->writeCode($handle, $lines);
	}
	
	public function writeOpCodes($handle) {
		$this->currentIndentationLevel = 0;
		
		$lines = array();
		$lines[] = "enum qb_opcode {";
		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			// if an op has a name ending in _SCA, there will also be one ending in _ELE, and possibly one in _MIO
			// create a constant without the _SCA part for reference purpose
			if(preg_match("/(\w+)_SCA$/", $name, $m)) {
				$baseName = $m[1];
				$lines[] = "QB_$baseName,";
				$lines[] = "QB_$name = QB_$baseName,";
			} else {
				$lines[] = "QB_$name,";
			}
		}
		$lines[] = "QB_OPCODE_COUNT";
		$lines[] = "};";		
		$this->writeCode($handle, $lines);
		
		$formats = $this->getInstructionFormats();
		$lines = array();
		$lines[] = "enum qb_instruction_format {";
		foreach($formats as $instr => $format) {
			$instrIndex = strtoupper($instr);
			$lines[] = "$instrIndex,";
		}
		$lines[] = "};";		
		$this->writeCode($handle, $lines);
	}
	
	public function writeOpInfo($handle) {
		$this->currentIndentationLevel = 0;
		
		$prevHandler = null;
		$lines = array();
		$lines[] = "const qb_op_info global_op_info[] = {";
		foreach($this->handlers as $index => $handler) {		
			$flags = array();
			$relationToPrevious = Handler::compare($handler, $prevHandler);

			if($relationToPrevious === false) {
				$variantIndex = $index + 1;
				do {
					// see if the next handler is a variant
					$nextHandler = isset($this->handlers[$variantIndex]) ? $this->handlers[$variantIndex] : null;
					$relationToNext = Handler::compare($handler, $nextHandler);
					if($relationToNext == "variant") {
						if($nextHandler->getAddressMode() == "ELE") {
							// same op using element addressing is available
							$flags[] = "QB_OP_VERSION_AVAILABLE_ELE";
						} else if($nextHandler->isMultipleData() == "MIO") {
							// multiple-input-output version of this op is available
							$flags[] = "QB_OP_VERSION_AVAILABLE_MIO";
						}
					}
					$variantIndex++;
				} while($relationToNext);
			}

			if($handler->getJumpTargetCount() > 0) {
				// op will redirect execution to another location 
				if($handler->getJumpTargetCount() == 2) {
					$flags[] = "QB_OP_BRANCH";
				} else {
					$flags[] = "QB_OP_JUMP";
				}
			}
			if($handler->needsLineNumber()) {
				// the line number needs to be packed into the instruction (for error reporting)
				$flags[] = "QB_OP_NEED_LINE_NUMBER";
			}
			if($handler->isVariableLength()) {
				$flags[] = "QB_OP_VARIABLE_LENGTH";
			}
			if($handler->performsWrapAround()) {
				$flags[] = "QB_OP_PERFORM_WRAP_AROUND";
			}
			$combined = ($flags) ? implode(" | ", $flags) : "0";

			$instr = $handler->getInstructionStructure();
			$instrIndex = strtoupper($instr);
			
			$name = $handler->getName();
			$lines[] = "// $name";
			$lines[] = "{	$instrIndex,	$combined	},";
			
			$prevHandler = $handler;
		}
		$lines[] = "};";
		$lines[] = "";
		$this->writeCode($handle, $lines);
		
		$formats = $this->getInstructionFormats();
		$lines = array();
		$lines[] = "const char *global_operand_codes[] = {";
		foreach($formats as $format) {
			$lines[] = "\"$format\",";
		}
		$lines[] = "};";
		$lines[] = "";
		$this->writeCode($handle, $lines);
		
		$lines = array();
		$lines[] = "const uint32_t global_instruction_lengths[] = {";
		foreach($formats as $struct => $format) {
			if($format != "v") {
				$lines[] = "sizeof($struct),";
			} else {
				$lines[] = "0,";
			}
		}
		$lines[] = "};";
		$lines[] = "";
		$this->writeCode($handle, $lines);
		
	}
	
	public function writeOpNames($handle) {
		$this->currentIndentationLevel = 0;
		
		$opnames = array();
		foreach($this->handlers as $handler) {
			$opnames[] = $handler->getName();
		}
		$folder = dirname(__FILE__);
		$zend_opnames = file("$folder/listings/zend_op_names.txt", FILE_IGNORE_NEW_LINES);
		$pbj_opnames = file("$folder/listings/pixel_bender_op_names.txt", FILE_IGNORE_NEW_LINES);

		fwrite($handle, "#ifdef HAVE_ZLIB\n");
		$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, true, true);
		$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, true, true);
		$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, true, true);
		fwrite($handle, "#else\n");
		$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, false, true);
		$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, false, true);
		$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, false, true);
		fwrite($handle, "#endif\n");
	}

	public function writeNativeCodeTables($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;
		
		$actions = array();
		foreach($this->handlers as $index => $handler) {
			$code = null;
			try {
				$lines = $handler->getAction();
				if($lines) {
					$code = $this->formatCode($lines);
				}
			} catch(Exception $e) {
			}
			$actions[$index] = $code;
		}

		$resultSizePossibilities = array();
		foreach($this->handlers as $index => $handler) {
			$expressions = $handler->getResultSizePossibilities();
			if($expressions) {
				if($expressions) {
					if(is_array($expressions)) {
						$list = "";
						foreach($expressions as $expr) {
							$list .= $expr . "\x00";
						}
					} else {
						$list = $expressions . "\x00";
					}
					$resultSizePossibilities[$index] = $list;
				}
			}
		}
		
		$resultSizeCalculations = array();
		foreach($this->handlers as $index => $handler) {
			$lines = $handler->getResultSizeCalculation();
			if($lines) {
				$resultSizeCalculations[$index] = $this->formatCode($lines);
			} else {
				$resultSizeCalculations[$index] = null;
			}
		}
		
		// create function declaration table and reverse look-up table
		$functionDeclarations = $this->getFunctionDeclarations();
		$functionIndices = array();
		$functionRefCounts = array();
		foreach($functionDeclarations as $index => &$decl) {
			if(preg_match('/(\w+)\s*\(/', $decl, $m)) {
				$functionName = $m[1];
				
				// get rid of ZEND_FASTCALL and ZEND_MACRO
				$decl = preg_replace('/\b(ZEND_FASTCALL|ZEND_MACRO)\s*/', '', $decl);
				$functionIndices[$functionName] = $index;
				$functionRefCounts[$index] = 0;
			}
		}
		unset($decl);
		
		// set up the function reference table
		$references = array();
		foreach($this->handlers as $handler) {
			$indices = array();
			$action = $handler->getCode();
			if($action) {
				if(is_scalar($action)) {
					$action = array($action);
				} else {
					$action = array_linearize($action);
				}
				foreach($action as $line) {
					// look for tokens that might be function calls
					$line = preg_replace('/#define \w+\s*/', '', $line);
					if(preg_match_all('/(\w+)\s*\(/', $line, $m)) {
						$names = $m[1];
						while($names) {
							$name = array_shift($names);
							if(isset($functionIndices[$name])) {
								$index = $functionIndices[$name];
								if(!in_array($index, $indices)) {
									$indices[] = $index;
									$functionRefCounts[$index]++;
								
									// if it's calling an inline function, we'll need to declare
									// functions called by the inline function
									$decl = $functionDeclarations[$index];
									if(strpos($decl, '{') !== false) {
										if(preg_match_all('/(\w+)\s*\(/', $decl, $m)) {
												$names = array_merge($names, $m[1]);
										}
									}
								}
							} else {
								switch($name) {
									case 'if':
									case 'for':
									case 'while':
									case 'sizeof':
									case 'return':
									case 'QB_G':
									case 'EG':
									case 'SWAP_LE_I16':
									case 'SWAP_LE_I32':
									case 'SWAP_LE_I64':
									case 'SWAP_BE_I16':
									case 'SWAP_BE_I32':
									case 'SWAP_BE_I64':
									case 'EXPECTED':
									case 'UNEXPECTED':
									case 'USE_TSRM':
										break;
									default:
										throw new Exception("Missing function declaration for $name");
								}
							}
						}
					}
				}
			}
			// indicate the end of the list
			$indices[] = 0xFFFFFFFF;
			$record = "";
			foreach($indices as $index) {
				$record .= pack("V", $index);
			}
			$references[] = $record;
		}
		
		// if a function is never called, then make it empty
		foreach($functionDeclarations as $index => &$decl) {
			if(!$functionRefCounts[$index]) {
				$decl = "";
			}
		}
		unset($decl);
		
		fwrite($handle, "#ifdef HAVE_ZLIB\n");
		$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_result_size_calculations", $resultSizeCalculations, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_result_size_possibilities", $resultSizePossibilities, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_prototypes", $functionDeclarations, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_references", $references, true, false);
		fwrite($handle, "#else\n");
		$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_result_size_calculations", $resultSizeCalculations, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_result_size_possibilities", $resultSizePossibilities, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_prototypes", $functionDeclarations, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_references", $references, false, false);
		fwrite($handle, "#endif\n");
	}
	
	public function writeNativeSymbolTable($handle, $compiler) {
		$this->setCompiler($compiler);
		$this->currentIndentationLevel = 0;
		
		// parse for declaration of helper functions 
		$functionDeclarations = $this->getFunctionDeclarations();
		
		// add intrinsics
		$folder = dirname(__FILE__);
		$intrinsic = file(($compiler == "MSVC") ? "$folder/listings/intrinsic_functions_msvc.txt" : "$folder/listings/intrinsic_functions_gcc.txt", FILE_IGNORE_NEW_LINES);
		foreach($intrinsic as $decl) {
			$functionDeclarations[] = $decl;
		}

		// figure out what the symbols are
		$symbols = array();
		foreach($functionDeclarations as $decl) {
			if(preg_match('/^\s*(.*?)\s*(\w+)\s*\(([^)]*)\)/', $decl, $m)) {
				$modifiers = $m[1];
				$functionName = $m[2];
				$target = null;
				
				if(strpos($decl, "{") === false) {
					if(preg_match('/\b(ZEND_FASTCALL|ZEND_MACRO)\b/', $modifiers)) {
						$isMacro = preg_match('/\b(ZEND_MACRO)\b/', $modifiers);
						$returnType = trim(preg_replace('/\b(ZEND_FASTCALL|ZEND_MACRO|NO_RETURN)\b/', '', $modifiers));
						$parameterDecls = $m[3];
						$target = "{$functionName}_symbol";
						if($parameterDecls) {
							$parameterDecls = preg_split('/\s*,\s+/', $parameterDecls);
							$parameters = array();
							foreach($parameterDecls as $parameterDecl) {
								$parameterDecl = trim($parameterDecl);
								if($parameterDecl == 'void') {
									// none
								} else if(preg_match('/\w+$/', $parameterDecl, $m)) {
									$parameters[] = $m[0];
								} else if(preg_match('/.../', $parameterDecl, $m)) {
									$parameters[] = $m[0];
								} else {
									throw new Exception("Unable to parse parameter declaration: $parameterDecl");
								}
							}
							$parameterList = implode(", ", $parameters);
							$parameterDecls = implode(", ", $parameterDecls);
						} else {
							$parameterList = "";
							$parameterDecls = "void";
						}
					
						// link name of function to itself when fastcall matches cdecl
						if(!$isMacro) {
							fwrite($handle, "#ifdef FASTCALL_MATCHES_CDECL\n");
							fwrite($handle, "#define $target	$functionName\n");
							fwrite($handle, "#else\n");
						}
						// create a cdecl wrapper for it
						fwrite($handle, "$returnType $target($parameterDecls) {\n");
						if($returnType != 'void') {
							fwrite($handle, "	return $functionName($parameterList);\n");
						} else {
							fwrite($handle, "	$functionName($parameterList);\n");
						}
						fwrite($handle, "}\n");
						if(!$isMacro) {
							fwrite($handle, "#endif\n");
						}
						fwrite($handle, "\n");
					} else {
						$target = $functionName;
					}
				} else {
					// a function body could be generated inside the object file for the inline function
					// need to indicate that the symbol is known 
					$target = "(void*) -1";
				}
				$symbols[$functionName] = $target;
			}
		}
		ksort($symbols);
		
		if($this->compiler == "MSVC") {
			// floor() is not constant in MSVC when intrinsic are used
			// it therefore cannot be used as an initializer
			// need it get the address at runtime instead
			$symbols["floor"] = "(void*) -1";
		}
	
		// print out prototypes of intrinsics
		foreach($intrinsic as $line) {
			fwrite($handle, "$line\n");
		}
		fwrite($handle, "\n");
		
		fwrite($handle, "qb_native_symbol global_native_symbols[] = {\n");
		foreach($symbols as $name => $symbol) {
			fwrite($handle, "	{	0,	\"$name\",	$symbol	},\n");
		}
		fwrite($handle, "};\n\n");
		$count = count($symbols);
		fwrite($handle, "uint32_t global_native_symbol_count = $count;\n\n");
	}

	public function writeNativeDebugStub($handle) {
		fwrite($handle, "#if NATIVE_COMPILE_ENABLED && ZEND_DEBUG\n");
		fwrite($handle, "#include \"qb_native_proc_debug.c\"\n");
		fwrite($handle, "#ifdef HAVE_NATIVE_PROC_RECORDS\n");
		fwrite($handle, "qb_native_proc_record *native_proc_table = native_proc_records;\n");
		fwrite($handle, "uint32_t native_proc_table_size = sizeof(native_proc_records) / sizeof(qb_native_proc_record);\n");
		fwrite($handle, "#else\n");
		fwrite($handle, "qb_native_proc_record *native_proc_table = NULL;\n");
		fwrite($handle, "uint32_t native_proc_table_size = 0;\n");
		fwrite($handle, "#endif\n");
		fwrite($handle, "#endif\n");
	}

	protected function setCompiler($compiler) {
		Handler::setCompiler($compiler);
		$this->compiler = $compiler;
	}
	
	protected function getFunctionDefinitions() {
		// add helper functions first
		$helperFunctions = array();
		foreach($this->handlers as $handler) {
			$helpers = $handler->getHelperFunctions();
			if($helpers) {
				foreach($helpers as $helper) {
					$line1 = $helper[0];
					if(!isset($helperFunctions[$line1])) {
						$helperFunctions[$line1] = $helper;
					}
				}
			}
		}
		ksort($helperFunctions);

		// add handler functions
		$handlerFunctions = array();
		foreach($this->handlers as $handler) {
			$function = $handler->getHandlerFunctionDefinition();
			if($function) {
				$line1 = $function[0];
				if(!isset($handlerFunctions[$line1])) {
					$handlerFunctions[$line1] = $function;
				}
			}
		}
		ksort($handlerFunctions);

		// add controller functions
		$controllerFunctions = array();
		foreach($this->handlers as $handler) {
			$function = $handler->getControllerFunctionDefinition();
			if($function) {
				$line1 = $function[0];
				if(!isset($controllerFunctions[$line1])) {
					$controllerFunctions[$line1] = $function;
				}
			}
		}

		// add dispatcher functions
		$dispatcherFunctions = array();
		foreach($this->handlers as $handler) {
			$function = $handler->getDispatcherFunctionDefinition();
			if($function) {
				$line1 = $function[0];
				if(!isset($dispatcherFunctions[$line1])) {
					$dispatcherFunctions[$line1] = $function;
				}
			}
		}
		
		ksort($dispatcherFunctions);
		
		return array_merge(array_values($helperFunctions), array_values($handlerFunctions), array_values($controllerFunctions), array_values($dispatcherFunctions));
	}
	
	protected function getFunctionDeclarations() {
		$functionDeclarations = array();
		
		// add the generated function first
		$functionDefinitions = $this->getFunctionDefinitions();
		
		// then prepend the list with ones that aren't generated
		$folder = dirname(__FILE__);
		$common = file("$folder/listings/function_prototypes.txt", FILE_IGNORE_NEW_LINES);
		$compilerSpecific = file(($this->compiler == "MSVC") ? "$folder/listings/function_prototypes_msvc.txt" : "$folder/listings/function_prototypes_gcc.txt", FILE_IGNORE_NEW_LINES);
		foreach(array_merge($common, $compilerSpecific) as $line) {
			array_unshift($functionDefinitions, array($line));
		}						

		// declare non-inlined functions first
		$inlinePattern = '/\b(inline|zend_always_inline)\b/';
		foreach($functionDefinitions as $lines) {
			$line1 = $lines[0];
			if(!preg_match($inlinePattern, $line1)) {
				// just the prototype--replace { and everything after it with ;
				$prototype = preg_replace('/\s*{.*$/', ';', $line1);					
				$functionDeclarations[] = $prototype;
			}
		}
		// then inlined functions
		foreach($functionDefinitions as $lines) {
			$line1 = $lines[0];
			if(preg_match($inlinePattern, $line1)) {
				// need all the lines
				$functionDeclarations[] = $this->formatCode($lines);
			}
		}
		return $functionDeclarations;
	}

	protected function getInstructionStructureDefinitions() {
		$structs = array();
		foreach($this->handlers as $handler) {
			$instr = $handler->getInstructionStructure();
			if(!isset($structs[$instr])) {
				$struct = $handler->getInstructionStructureDefinition();
				$structs[$instr] = $struct;
			}
		}
		ksort($structs);
		return $structs;
	}

	protected function getInstructionFormats() {
		$formats = array();
		foreach($this->handlers as $handler) {
			$instr = $handler->getInstructionStructure();
			if(!isset($formats[$instr])) {
				$format = $handler->getInstructionFormat();
				$formats[$instr] = $format;
			}
		}
		ksort($formats);
		return $formats;
	}

	protected function writeCode($handle, $lines) {
		foreach($lines as $line) {
			if($line !== null) {
				if(is_array($line)) {
					$this->writeCode($handle, $line);
				} else {
					$this->currentIndentationLevel -= substr_count($line, "}");
					if(!preg_match('/^#/', $line)) {
						$line = str_repeat("\t", $this->currentIndentationLevel) . $line;
					}
					fwrite($handle, "$line\n");
					if($this->currentIndentationLevel < 0) {
						throw new Exception("There's a problem with the code indentation\n");
					}
					$this->currentIndentationLevel += substr_count($line, "{");
				}
			}
		}
	}
	
	protected function formatCode($lines) {
		if(is_array($lines)) {
			$this->currentIndentationLevel = 0;
			$code = $this->formatLines($lines);
			return $code;
		} else {
			return "$lines\n";
		}
	}
	
	protected function formatLines($lines) {
		$code = "";
		foreach($lines as $line) {
			if($line !== null) {
				if(is_array($line)) {
					$code .= $this->formatLines($line);
				} else {
					$closeBracketCount = substr_count($line, "}");
					$openBracketCount = substr_count($line, "{");
					if($closeBracketCount == $openBracketCount) {
						$closeBracketCount = $openBracketCount = 0;
					}
					$this->currentIndentationLevel -= $closeBracketCount;
					if(!preg_match('/^#/', $line)) {
						$line = str_repeat("\t", $this->currentIndentationLevel) . $line;
					}
					$code .= "$line\n";
					if($this->currentIndentationLevel < 0) {
						throw new Exception("There's a problem with the code indentation\n");
					}
					$this->currentIndentationLevel += $openBracketCount;
				}
			}
		}
		return $code;
	}

	protected function writeCompressTable($handle, $name, $table, $deflate, $includeZeroTerminator) {
		// find out to which indices each item belong (multiple associations are expected)
		$refTable = array();
		$maxIndex = 0;
		foreach($table as $index => $item) {
			$refTable[$item][] = $index;
			if($index > $maxIndex) {
				$maxIndex = $index;
			}
		}
		
		// pack the records
		$records = array();	
		$totalDataLength = 0;
		foreach($refTable as $data => $indices) {
			$indexCount = count($indices);
			$dataLength = strlen($data);
			if($includeZeroTerminator) {
				$dataLength++;
				$data .= "\0";
			}
			// little endian 32-bit  integer
			$record = pack("V", $indexCount);
			foreach($indices as $index) {
				$record .= pack("V", $index);
			}
			$record .= pack("V", $dataLength);
			$record .= $data;
			$records[] = $record;
			$totalDataLength += $dataLength;
		}
		$data = implode("", $records);
		$uncompressedLength = strlen($data);
		if($deflate) {
			$compressedData = gzdeflate($data, 9);
			$compressedLength = strlen($compressedData);
			if((double) $compressedLength / $uncompressedLength < 0.75) {
				// use the deflated version only if the compression ratio is better than 75%
				$data = $compressedData;
			} else {
				$compressedLength = $uncompressedLength;
			}
		} else {
			$compressedLength = $uncompressedLength;
		}
		$itemCount = $maxIndex + 1;
		$tableData = pack("V*", $compressedLength, $uncompressedLength, $totalDataLength, $itemCount) . $data;
		
		$len = strlen($tableData) + 1;
		fwrite($handle, "const char {$name}[{$len}] = \"");
		for($i = 0, $len = strlen($tableData); $i < $len; $i++) {
			fwrite($handle, sprintf("\x%02X", ord($tableData[$i])));
		}
		fwrite($handle, "\";\n");
	}
	
	protected function addHandlers() {
		foreach($this->elementTypes as $elementType) {
			$this->addFlowControlHandlers($elementType);
			$this->addIncrementDecrementHandlers($elementType);
			$this->addArithmeticHandlers($elementType);
			$this->addAssignmentHandlers($elementType);
			$this->addComparisonHandlers($elementType);
			$this->addBitwiseHandlers($elementType);
			$this->addLogicalHandlers($elementType);
			$this->addCastHandlers($elementType);
			$this->addMathHandlers($elementType);
			$this->addStringHandlers($elementType);
			$this->addArrayHandlers($elementType);
			$this->addSamplingHandlers($elementType);
			$this->addMatrixHandlers($elementType);
			$this->addComplexNumberHandlers($elementType);
		}
		$this->addDebugHandlers();
	}

	protected function addArithmeticHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Add("ADD", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Subtract("SUB", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Multiply("MUL", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Divide("DIV", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Modulo("MOD", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$width = (int) substr($elementType, 1);
			if($width >= 32) {
				$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, $addressMode);
			}
		}
		if($float) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new FlooredDivisionModulo("MOD_FLR", $elementType, $addressMode);
			}		
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Negate("NEG", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addAssignmentHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Copy("MOV", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addIncrementDecrementHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Increment("INC", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Decrement("DEC", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addComparisonHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Equal("EQ", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new NotEqual("NE", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new LessThan("LT", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new LessThanOrEqual("LE", $elementType, $addressMode);
		}
		if(!$unsigned) {
			$this->handlers[] = new SetEqual("EQ_SET", $elementTypeNoSign);
			$this->handlers[] = new SetNotEqual("NE_SET", $elementTypeNoSign);
		}
		$this->handlers[] = new SetLessThan("LT_SET", $elementType);
		$this->handlers[] = new SetLessThanOrEqual("LE_SET", $elementType);
		if($elementTypeNoSign == "I32") {
			$this->handlers[] = new SetNot("NOT_SET", $elementTypeNoSign);
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new SetAny("ANY", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new SetAll("ALL", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addBitwiseHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if(!$float) {
			$unsigned = preg_match('/^U/', $elementType);
			$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
			if(!$unsigned) {
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new BitwiseAnd("BW_AND", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new BitwiseOr("BW_OR", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new BitwiseXor("BW_XOR", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new BitwiseNot("BW_NOT", $elementTypeNoSign, $addressMode);
				}
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ShiftLeft("SHL", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ShiftRight("SHR", $elementType, $addressMode);
			}
		}
	}

	protected function addLogicalHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			if($elementTypeNoSign == "I32") {
				$this->handlers[] = new LogicalAnd("AND", $elementTypeNoSign);
				$this->handlers[] = new LogicalOr("OR", $elementTypeNoSign);
				$this->handlers[] = new LogicalXor("XOR", $elementTypeNoSign);
				$this->handlers[] = new LogicalNot("NOT", $elementTypeNoSign);
			}
		}
	}

	protected function addCastHandlers($elementType) {
		$elementUnsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		foreach($this->elementTypes as $destElementType) {
			$destUnsigned = preg_match('/^U/', $destElementType);
			$destTypeNoSign = preg_replace('/^S/', "I", $destElementType);
			if($elementType != $destElementType && $elementTypeNoSign != $destTypeNoSign) {
				$float = preg_match('/^F/', $elementType) || preg_match('/^F/', $destElementType);
				$promotion = (int) substr($elementType, 1) < (int) substr($destTypeNoSign, 1);
				if($float) {
					// sign matters when floating points are involved
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new Cast("MOV", "{$elementType}_{$destElementType}", $addressMode);
					}
				} else if($promotion && !$destUnsigned) {
					// sign matters when promoting from lower-rank integer type to higher-rank type
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new Cast("MOV", "{$elementType}_{$destTypeNoSign}", $addressMode);
					}
				} else if(!$elementUnsigned && !$destUnsigned) {
					// sign doesn't matter
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new Cast("MOV", "{$elementTypeNoSign}_{$destTypeNoSign}", $addressMode);
					}
				}
			}
		}
		if(!$elementUnsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new BooleanCast("BOOL", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addMathHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$elementUnsigned = preg_match('/^U/', $elementType);
		if(!$elementUnsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Abs("ABS", $elementType, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Min("MIN", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Max("MAX", $elementType, $addressMode);
		}
		if($float) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Sin("SIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ASin("ASIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Cos("COS", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ACos("ACOS", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Tan("TAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ATan("ATAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ATan2("ATAN2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Sinh("SINH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ASinh("ASINH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Cosh("COSH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ACosh("ACOSH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Tanh("TANH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ATanh("ATANH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Ceil("CEIL", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Floor("FLOOR", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Round("ROUND", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Log("LOG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Log1P("LOG1P", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Log2("LOG2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Log10("LOG10", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Exp("EXP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ExpM1("EXPM1", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Exp2("EXP2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Pow("POW", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Sqrt("SQRT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Hypot("HYPOT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new LCG("LCG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new IsFinite("FIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new IsInfinite("INF", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new IsNaN("NAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new RSqrt("RSQRT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Clamp("CLAMP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Fract("FRACT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Mix("MIX", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Sign("SIGN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Step("STEP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SmoothStep("SSTEP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new RadianToDegree("RAD2DEG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new DegreeToRadian("DEG2RAD", $elementType, $addressMode);
			}
		} else {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Random("RAND", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new RandomMT("MT_RAND", $elementType, $addressMode);
			}
		}
	}

	protected function addFlowControlHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!in_array("Jump", array_map("get_class", $this->handlers))) {
			$this->handlers[] = new NOP("NOP");
			$this->handlers[] = new Jump("JMP");
			$this->handlers[] = new Leave("RET");
			$this->handlers[] = new Terminate("EXIT", "I32");
			//$this->handlers[] = new FunctionCall("FCALL", "SCA");
			//$this->handlers[] = new FunctionCall("FCALL", "MIX");
			$this->handlers[] = new BranchIfStaticVariablesInitialized("IF_INIT");
		}
		$branchHandlers = array();
		if(!$unsigned) {
			if($elementTypeNoSign == "I32") {
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new BranchOnTrue("IF_T", $elementTypeNoSign, $addressMode);
				}
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new BranchOnFalse("IF_F", $elementTypeNoSign, $addressMode);
				}
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new BranchOnEqual("IF_EQ", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new BranchOnNotEqual("IF_NE", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new BranchOnLessThan("IF_LT", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new BranchOnLessThanOrEqual("IF_LE", $elementType, $addressMode);
		}
	}

	protected function addStringHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$float = preg_match('/^F/', $elementType);
		if($elementType == "U08") {
			$this->handlers[] = new PrintString("PRN_STR", $elementType);
			//$this->handlers[] = new ConcatString("CAT_STR", $elementType);
			
		}
		if($elementType == "U16" || $elementType == "U32") {
			//$this->handlers[] = new UTF8Decode("UTF8_DEC", $elementType);
			//$this->handlers[] = new UTF8Encode("UTF8_ENC", $elementType);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new PrintVariable("PRN", $elementType, $addressMode);
		}
		$this->handlers[] = new PrintMultidimensionalVariable("PRN_DIM", $elementType);
		foreach($this->addressModes as $addressMode) {
			//$this->handlers[] = new ConcatVariable("CAT", $elementType, $addressMode);
		}
		//$this->handlers[] = new ConcatMultidimensionalVariable("CAT_DIM", $elementType);
		
		if(!$unsigned && $elementTypeNoSign != "I08") {
			/*
			foreach($this->scalarAddressModes as $addressMode) {		
				$this->handlers[] = new Pack("PACK_LE", "{$elementTypeNoSign}_LE", $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new Pack("PACK_BE", "{$elementTypeNoSign}_BE", $addressMode, "BE");
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new Unpack("UNPACK_LE", "{$elementTypeNoSign}_LE", $addressMode, "LE");
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new Unpack("UNPACK_BE", "{$elementTypeNoSign}_BE", $addressMode, "BE");
			}
			*/
		}
	}
	
	protected function addArrayHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$this->handlers[] = new Sort("SORT", $elementType);
		$this->handlers[] = new ReverseSort("RSORT", $elementType);
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ArrayMin("AMIN", $elementType, $addressMode);
			$this->handlers[] = new ArrayMax("AMAX", $elementType, $addressMode);
			$this->handlers[] = new ArrayProduct("APROD", $elementType, $addressMode);
			$this->handlers[] = new ArraySum("ASUM", $elementType, $addressMode);
			$this->handlers[] = new Range("RANGE", $elementType, $addressMode);
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				//$this->handlers[] = new ArraySearch("AFIND", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new SubarrayPosition("APOS", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new SubarrayPositionFromEnd("ARPOS", $elementTypeNoSign, $addressMode);
			}
			$this->handlers[] = new ArrayReverse("AREV", $elementTypeNoSign);
			//$this->handlers[] = new ArrayInsert("AINS", $elementTypeNoSign);
			$this->handlers[] = new ArrayUnique("AUNIQ", $elementTypeNoSign);
			$this->handlers[] = new ArrayColumn("ACOL", $elementTypeNoSign);
			$this->handlers[] = new ArrayDifference("ADIFF", $elementTypeNoSign);
			$this->handlers[] = new ArrayIntersect("AISECT", $elementTypeNoSign);
			$this->handlers[] = new Shuffle("SHUFFLE", $elementTypeNoSign);
			//$this->handlers[] = new ArrayResize("ARESIZE", $elementTypeNoSign);
			$this->handlers[] = new ArrayPad("APAD", $elementTypeNoSign);
		}
		if($elementType == 'U32') {
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new ArrayRandom("ARAND", $elementType, $addressMode);
			}
		}
	}
	
	protected function addSamplingHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if($float) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleNearest("SAMPLE_NN", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleNearest("SAMPLE_NN", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			$this->handlers[] = new AlphaBlend("BLEND", $elementType);
			$this->handlers[] = new ApplyPremultiplication("PREMULT", $elementType, 4);
			$this->handlers[] = new ApplyPremultiplication("PREMULT", $elementType, 4, true);
			$this->handlers[] = new RemovePremultiplication("UNPREMULT", $elementType, 4);
			$this->handlers[] = new RemovePremultiplication("UNPREMULT", $elementType, 4, true);
			$this->handlers[] = new RGB2HSV("RGB2HSV", $elementType, 3);
			$this->handlers[] = new RGB2HSV("RGB2HSV", $elementType, 3, true);
			$this->handlers[] = new RGB2HSV("RGB2HSV", $elementType, 4);
			$this->handlers[] = new RGB2HSV("RGB2HSV", $elementType, 4, true);
			$this->handlers[] = new HSV2RGB("HSV2RGB", $elementType, 3);
			$this->handlers[] = new HSV2RGB("HSV2RGB", $elementType, 3, true);
			$this->handlers[] = new HSV2RGB("HSV2RGB", $elementType, 4);
			$this->handlers[] = new HSV2RGB("HSV2RGB", $elementType, 4, true);
			$this->handlers[] = new RGB2HSL("RGB2HSL", $elementType, 3);
			$this->handlers[] = new RGB2HSL("RGB2HSL", $elementType, 3, true);
			$this->handlers[] = new RGB2HSL("RGB2HSL", $elementType, 4);
			$this->handlers[] = new RGB2HSL("RGB2HSL", $elementType, 4, true);
			$this->handlers[] = new HSL2RGB("HSL2RGB", $elementType, 3);
			$this->handlers[] = new HSL2RGB("HSL2RGB", $elementType, 3, true);
			$this->handlers[] = new HSL2RGB("HSL2RGB", $elementType, 4);
			$this->handlers[] = new HSL2RGB("HSL2RGB", $elementType, 4, true);
		}
	}
	
	protected function addMatrixHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if($float) {
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 4);
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 4, true);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 4);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 4, true);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 4);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 4, true);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 4);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 4, true);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 4);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 4, true);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Determinant("MDET", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new DotProduct("DOT", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Length("LEN", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Distance("DIS", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			$this->handlers[] = new Normalize("NORM", $elementType, 4);
			$this->handlers[] = new Normalize("NORM", $elementType, 4, true);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 4);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 4, true);
			$this->handlers[] = new FaceForward("FORE", $elementType, 4);
			$this->handlers[] = new FaceForward("FORE", $elementType, 4, true);
			$this->handlers[] = new Reflect("REFL", $elementType, 4);
			$this->handlers[] = new Reflect("REFL", $elementType, 4, true);
			$this->handlers[] = new Refract("REFR", $elementType, 4);
			$this->handlers[] = new Refract("REFR", $elementType, 4, true);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 4);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 4, true);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 4);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 4, true);
			
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 4);
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 4, true);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 4);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 4, true);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 4);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 4, true);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 4);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 4, true);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 4);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 4, true);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 4);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 4, true);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 4);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 4, true);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 4);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 4, true);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 4);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 4, true);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 4);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 4, true);

			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 3);
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 3, true);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 3);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 3, true);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 3);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 3, true);
			if($elementType == "F32") {
				// these are used by Pixel Bender only
				$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM_PAD", $elementType, 3);
				$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM_PAD", $elementType, 3);
				$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM_PAD", $elementType, 3);
			}
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 3);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 3, true);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 3);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 3, true);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Determinant("MDET", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new DotProduct("DOT", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Length("LEN", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Distance("DIS", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			$this->handlers[] = new Normalize("NORM", $elementType, 3);
			$this->handlers[] = new Normalize("NORM", $elementType, 3, true);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 3);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 3, true);
			$this->handlers[] = new FaceForward("FORE", $elementType, 3);
			$this->handlers[] = new FaceForward("FORE", $elementType, 3, true);
			$this->handlers[] = new Reflect("REFL", $elementType, 3);
			$this->handlers[] = new Reflect("REFL", $elementType, 3, true);
			$this->handlers[] = new Refract("REFR", $elementType, 3);
			$this->handlers[] = new Refract("REFR", $elementType, 3, true);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 3);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 3, true);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 3);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 3, true);
			
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 3);
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 3, true);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 3);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 3, true);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 3);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 3, true);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 3);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 3, true);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 3);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 3, true);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 3);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 3, true);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 3);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 3, true);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 3);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 3, true);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 3);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 3, true);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 3);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 3, true);
			
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 2);
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, 2, true);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 2);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, 2, true);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 2);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, 2, true);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 2);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, 2, true);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 2);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, 2, true);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Determinant("MDET", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new DotProduct("DOT", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {			
				$this->handlers[] = new Length("LEN", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Distance("DIS", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			$this->handlers[] = new Normalize("NORM", $elementType, 2);
			$this->handlers[] = new Normalize("NORM", $elementType, 2, true);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 2);
			$this->handlers[] = new CrossProduct("CROSS", $elementType, 2, true);
			$this->handlers[] = new FaceForward("FORE", $elementType, 2);
			$this->handlers[] = new FaceForward("FORE", $elementType, 2, true);
			$this->handlers[] = new Reflect("REFL", $elementType, 2);
			$this->handlers[] = new Reflect("REFL", $elementType, 2, true);
			$this->handlers[] = new Refract("REFR", $elementType, 2);
			$this->handlers[] = new Refract("REFR", $elementType, 2, true);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 2);
			$this->handlers[] = new TransformVector("TRAN_CM", $elementType, 2, true);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 2);
			$this->handlers[] = new TransformVector("TRAN_RM", $elementType, 2, true);
			
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 2);
			$this->handlers[] = new Copy("MOV", $elementType, "ARR", 2, true);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 2);
			$this->handlers[] = new Add("ADD", $elementType, "ARR", 2, true);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 2);
			$this->handlers[] = new Subtract("SUB", $elementType, "ARR", 2, true);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 2);
			$this->handlers[] = new Multiply("MUL", $elementType, "ARR", 2, true);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 2);
			$this->handlers[] = new Divide("DIV", $elementType, "ARR", 2, true);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 2);
			$this->handlers[] = new Modulo("MOD", $elementType, "ARR", 2, true);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 2);
			$this->handlers[] = new Negate("NEG", $elementType, "ARR", 2, true);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 2);
			$this->handlers[] = new Increment("INC", $elementType, "ARR", 2, true);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 2);
			$this->handlers[] = new Decrement("DEC", $elementType, "ARR", 2, true);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 2);
			$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, "ARR", 2, true);
			
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, "variable");
			$this->handlers[] = new MultiplyMatrixByMatrix("MUL_MM_CM", $elementType, "variable", true);
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, "variable");
			$this->handlers[] = new MultiplyMatrixByVector("MUL_MV_CM", $elementType, "variable", true);
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, "variable");
			$this->handlers[] = new MultiplyVectorByMatrix("MUL_VM_CM", $elementType, "variable", true);
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, "variable");
			$this->handlers[] = new TransposeMatrix("MTRAN", $elementType, "variable", true);
			$this->handlers[] = new InvertMatrix("MINV", $elementType, "variable");
			$this->handlers[] = new InvertMatrix("MINV", $elementType, "variable", true);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Determinant("MDET", $elementType, $addressMode, "variable", ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new DotProduct("DOT", $elementType, $addressMode, "variable", ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Length("LEN", $elementType, $addressMode, "variable", ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Distance("DIS", $elementType, $addressMode, "variable", ($addressMode == "ARR"));
			}
			$this->handlers[] = new Normalize("NORM", $elementType, "variable");
			$this->handlers[] = new Normalize("NORM", $elementType, "variable", true);
			$this->handlers[] = new FaceForward("FORE", $elementType, "variable");
			$this->handlers[] = new FaceForward("FORE", $elementType, "variable", true);
			$this->handlers[] = new Reflect("REFL", $elementType, "variable");
			$this->handlers[] = new Reflect("REFL", $elementType, "variable", true);
			$this->handlers[] = new Refract("REFR", $elementType, "variable");
			$this->handlers[] = new Refract("REFR", $elementType, "variable", true);
		}
	}

	protected function addComplexNumberHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if($float) {
			$this->handlers[] = new ComplexAbs("CABS", $elementType, 2);
			$this->handlers[] = new ComplexAbs("CABS", $elementType, 2, true);
			$this->handlers[] = new ComplexArgument("CARG", $elementType, 2);
			$this->handlers[] = new ComplexArgument("CARG", $elementType, 2, true);
			$this->handlers[] = new ComplexMultiply("CMUL", $elementType, 2);
			$this->handlers[] = new ComplexMultiply("CMUL", $elementType, 2, true);
			$this->handlers[] = new ComplexDivide("CDIV", $elementType, 2);
			$this->handlers[] = new ComplexDivide("CDIV", $elementType, 2, true);
			$this->handlers[] = new ComplexExp("CEXP", $elementType, 2);
			$this->handlers[] = new ComplexExp("CEXP", $elementType, 2, true);
			$this->handlers[] = new ComplexLog("CLOG", $elementType, 2);
			$this->handlers[] = new ComplexLog("CLOG", $elementType, 2, true);
			$this->handlers[] = new ComplexSquareRoot("CSQRT", $elementType, 2);
			$this->handlers[] = new ComplexSquareRoot("CSQRT", $elementType, 2, true);
			$this->handlers[] = new ComplexPow("CPOW", $elementType, 2);
			$this->handlers[] = new ComplexPow("CPOW", $elementType, 2, true);
			
			$this->handlers[] = new ComplexSin("CSIN", $elementType, 2);
			$this->handlers[] = new ComplexSin("CSIN", $elementType, 2, true);
			$this->handlers[] = new ComplexCos("CCOS", $elementType, 2);
			$this->handlers[] = new ComplexCos("CCOS", $elementType, 2, true);
			$this->handlers[] = new ComplexTan("CTAN", $elementType, 2);
			$this->handlers[] = new ComplexTan("CTAN", $elementType, 2, true);
			$this->handlers[] = new ComplexSinh("CSINH", $elementType, 2);
			$this->handlers[] = new ComplexSinh("CSINH", $elementType, 2, true);
			$this->handlers[] = new ComplexCosh("CCOSH", $elementType, 2);
			$this->handlers[] = new ComplexCosh("CCOSH", $elementType, 2, true);
			$this->handlers[] = new ComplexTanh("CTANH", $elementType, 2);
			$this->handlers[] = new ComplexTanh("CTANH", $elementType, 2, true);
		}
	}
	
	protected function addDebugHandlers() {
		$this->handlers[] = new ExtensionOp("EXT", "U32");
	}
}

?>