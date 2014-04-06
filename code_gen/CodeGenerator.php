<?php

class CodeGenerator {
	protected $handlers = array();
	protected $elementTypes = array("U32", "S32", "F32", "F64", "S08", "U08", "S16", "U16", "S64", "U64");
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
	
	public function writeTypeDeclarations($handle) {
		$this->currentIndentationLevel = 0;
	
		$lines[] = "#if defined(USE_TAIL_CALL_INTERPRETER_LOOP) || defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)";
		$lines[] = "typedef void * qb_op_handler;";
		$lines[] = "#else";
		$lines[] = "typedef int32_t qb_op_handler;";
		$lines[] = "#endif";
		$lines[] = "";

		$lines[] = "#if defined(__sun) || defined(__sun__)";
		$lines[] = "#pragma pack(1)";
		$lines[] = "#else";
		$lines[] = "#pragma pack(push, 1)";
		$lines[] = "#endif";
		$lines[] = "";
		
		$branchTableEntry = array(
			"typedef struct qb_branch_table_entry {",
				"qb_op_handler next_handler;",
				"int8_t *instruction_pointer;",
			"} qb_branch_table_entry;",
		);
		$lines[] = $branchTableEntry;
		$lines[] = "";
		
		$structs = $this->getInstructionStructureDefinitions();
		foreach($structs as $struct) {
			if($struct) {
				$lines[] = $struct;
				$lines[] = "";
			}
		}
		
		$lines[] = "#if defined(__sun) || defined(__sun__)";
		$lines[] = "#	pragma pack()";
		$lines[] = "#else";
		$lines[] = "#	pragma pack(pop)";
		$lines[] = "#endif";

		$lines[] = "#if defined(USE_TAIL_CALL_INTERPRETER_LOOP) || defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)";
		$lines[] = "extern qb_op_handler op_handlers[];";
		$lines[] = "#endif";
		$lines[] = "";
		$this->writeCode($handle, $lines);
	}
	
	public function writeFunctionPrototypes($handle) {
		$this->currentIndentationLevel = 0;
		$lines = array();

		$functions = $this->getFunctionDefinitions();
		$inlinePattern = '/\b(inline|zend_always_inline)\b/';

		// add non-inline functions first
		foreach($functions as $function) {
			$line1 = $function[0];
			
			if(!preg_match($inlinePattern, $line1)) {
				$prototype = preg_replace('/\s*{\s*$/', ';', $line1);
				$lines[] = $prototype;
			}
		}
		$lines[] = "";
		
		// add inline-functions
		foreach($functions as $function) {
			$line1 = $function[0];
			
			if(preg_match($inlinePattern, $line1)) {
				$lines[] = $function;
				$lines[] = "";
			}
		}
		$this->writeCode($handle, $lines);
	}

	public function writeFunctionDefinitions($handle) {
		$this->currentIndentationLevel = 0;
		$lines = array();
		
		$functions = $this->getFunctionDefinitions();
		foreach($functions as $function) {
			$line1 = $function[0];
		
			if(!preg_match('/\b(inline|zend_always_inline)\b/', $line1)) {
				$lines[] = $function;
				$lines[] = "";
			}
		}
		$this->writeCode($handle, $lines);
	}

	protected function writeSwitchLoop($handle) {
		$lines = array();
		$lines[] = "void qb_main(qb_interpreter_context *__restrict cxt) {";
		$lines[] = 		"register qb_opcode handler = ((qb_instruction *) cxt->instruction_pointer)->next_handler;";
		$lines[] = 		"register int8_t *__restrict ip = cxt->instruction_pointer + sizeof(qb_instruction);";
		$lines[] = "#ifdef _WIN32";
		$lines[] =		"uint32_t windows_timeout_check_counter = 0;";
		$lines[] = 		"volatile zend_bool *windows_timed_out_pointer = cxt->windows_timed_out_pointer;";
		$lines[] = "#endif";
		$lines[] =		"";
		$lines[] = 		"while(1) switch(handler) {";

		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			$instr = $handler->getInstructionStructure();
			$opCount = $handler->getOperandCount();
			$targetCount = $handler->getJumpTargetCount();

			$lines[] =		"case QB_$name:";
			$lines[] = 		$handler->getMacroDefinitions();
			$lines[] = 		"{";
		
			if($targetCount == 0 || $targetCount == 1) {
				// set next handler
				$lines[] =		"handler = INSTR->next_handler;";
			} else if($targetCount == 2) {
				// assume the first branch will be taken
				$lines[] = 		"int condition;";
				$lines[] = 		"handler = INSTR->next_handler1;";
			} else if($targetCount > 2) {
				// branch table
				$lines[] = 		"unsigned int offset;";
			}
				
			$lines[] = 			$handler->getAction();

			if($targetCount == 0) {
				// move the instruction pointer over this one
				$lines[] = 		"ip += sizeof($instr);";
			} else if($targetCount == 1) {
				// go to the jump target
				$lines[] = 		"ip = INSTR->instruction_pointer;";
			} else if($targetCount == 2) {
				// set the instruction pointer to pointer 1, if condition is true
				// otherwise update the next handler and set ip to pointer 2
				$lines[] = 		"if(condition) {";
				$lines[] = 			"ip = INSTR->instruction_pointer1;";
				$lines[] = 		"} else {";
				$lines[] = 			"handler = INSTR->next_handler2;";
				$lines[] = 			"ip = INSTR->instruction_pointer2;";
				$lines[] = 		"}";
			}  else if($targetCount > 2) {
				$lines[] = 		"handler = INSTR->branch_table[offset].next_handler;";
				$lines[] = 		"ip = INSTR->branch_table[offset].instruction_pointer;";
			}

			if($targetCount == 1 || $targetCount == 2) {
				$lines[] = 		"#ifdef _WIN32";
				$lines[] = 		"if(UNEXPECTED(windows_timeout_check_counter++ == 1048576)) {";
				$lines[] =			"windows_timeout_check_counter = 0;";
				$lines[] = 			"if(*windows_timed_out_pointer) {";
				$lines[] =				"cxt->exit_type = QB_VM_TIMEOUT;";
				$lines[] = 				"return;";
				$lines[] =			"}";
				$lines[] = 		"}";
				$lines[] = 		"#endif";
			}
			$lines[] = 		"}";
			$lines[] = 		$handler->getMacroUndefinitions();
			$lines[] =		"break;";
			$lines[] =		"";
		}

		$lines[] = "#ifdef _MSC_VER";		
		$lines[] = 			"default:";
		$lines[] = 				"__assume(0);";	
		$lines[] = "#endif";
		$lines[] = "#ifdef __GNUC__";
		$lines[] =			"case QB_OPCODE_COUNT: break;";
		$lines[] = "#endif";
		$lines[] = 		"}";
		$lines[] = "}";

		$this->writeCode($handle, $lines);
	}

	protected function writeComputedGotoLoop($handle) {
		$lines = array();
		$lines[] = "void qb_main(qb_interpreter_context *__restrict cxt) {";
		$lines[] =		"if(cxt) {";
		$lines[] = 			"register qb_op_handler __restrict handler = ((qb_instruction *) cxt->instruction_pointer)->next_handler;";
		$lines[] = 			"register int8_t *__restrict ip = cxt->instruction_pointer + sizeof(qb_instruction);";		
		$lines[] = "#ifdef _WIN32";
		$lines[] =			"uint32_t windows_timeout_check_counter = 0;";
		$lines[] = 			"volatile zend_bool *windows_timed_out_pointer = cxt->windows_timed_out_pointer;";
		$lines[] = "#endif";
		$lines[] =			"";
		$lines[] = 			"goto *handler;";
		$lines[] = 			"";

		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			$instr = $handler->getInstructionStructure();
			$opCount = $handler->getOperandCount();
			$targetCount = $handler->getJumpTargetCount();

			$lines[] =		"label_$name:";
			$lines[] = 		$handler->getMacroDefinitions();
			$lines[] = 		"{";
		
			if($targetCount == 0 || $targetCount == 1) {
				// set next handler
				$lines[] =		"handler = INSTR->next_handler;";
			} else if($targetCount == 2) {
				// assume the first branch will be taken
				$lines[] = 		"int condition;";
				$lines[] = 		"handler = INSTR->next_handler1;";
			} else if($targetCount > 2) {
				// branch table
				$lines[] = 		"unsigned int offset;";
			}
				
			$lines[] = 			$handler->getAction();

			if($targetCount == 0) {
				// move the instruction pointer over this one
				$lines[] = 		"ip += sizeof($instr);";
			} else if($targetCount == 1) {
				// go to the jump target
				$lines[] = 		"ip = INSTR->instruction_pointer;";
			} else if($targetCount == 2) {
				// set the instruction pointer to pointer 1, if condition is true
				// otherwise update the next handler and set ip to pointer 2
				$lines[] = 		"if(condition) {";
				$lines[] = 			"ip = INSTR->instruction_pointer1;";
				$lines[] = 		"} else {";
				$lines[] = 			"handler = INSTR->next_handler2;";
				$lines[] = 			"ip = INSTR->instruction_pointer2;";
				$lines[] = 		"}";
			}  else if($targetCount > 2) {
				$lines[] = 		"handler = INSTR->branch_table[offset].next_handler;";
				$lines[] = 		"ip = INSTR->branch_table[offset].instruction_pointer;";
			}

			if($targetCount == 1 || $targetCount == 2) {
				$lines[] = 		"#ifdef _WIN32";
				$lines[] = 		"if(UNEXPECTED(windows_timeout_check_counter++ == 1048576)) {";
				$lines[] =			"windows_timeout_check_counter = 0;";
				$lines[] = 			"if(*windows_timed_out_pointer) {";
				$lines[] =				"cxt->exit_type = QB_VM_TIMEOUT;";
				$lines[] = 				"return;";
				$lines[] =			"}";
				$lines[] = 		"}";
				$lines[] = 		"#endif";
			}

			// go to the next instruction unless the function is returning
			if($targetCount != -1) {
				$lines[] =		"goto *handler;";
			}
			$lines[] = 		"}";
			$lines[] = 		$handler->getMacroUndefinitions();
			$lines[] =		"";
		}
		$lines[] = 		"} else {";

		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			$lines[] = 		"op_handlers[QB_$name] = &&label_$name;";
		}

		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "";
		$lines[] = "qb_op_handler *op_handlers[QB_OPCODE_COUNT];";

		$this->writeCode($handle, $lines);
	}

	protected function writeTailCallLoop($handle) {
		$lines = array();

		$lines[] = "#define return		longjmp(cxt->tail_call_exit)";
		$lines[] = "";

		$lines[] = "typedef void (*qb_tc_handler)(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip);";
		$lines[] = "";

		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			$instr = $handler->getInstructionStructure();
			$opCount = $handler->getOperandCount();
			$targetCount = $handler->getJumpTargetCount();

			$lines[] = "static int8_t *qb_tc_$name(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip) {";
			$lines[] = 		$handler->getMacroDefinitions();
			$lines[] = 		"register qb_tc_handler __restrict handler = ((qb_instruction *) cxt->instruction_pointer)->next_handler;";
			$lines[] = 		"";
		
			if($targetCount == 0 || $targetCount == 1) {
				// set next handler
				$lines[] =	"handler = INSTR->next_handler;";
			} else if($targetCount == 2) {
				// assume the first branch will be taken
				$lines[] = 	"int condition;";
				$lines[] = 	"handler = INSTR->next_handler1;";
			} else if($targetCount > 2) {
				// branch table
				$lines[] = 	"unsigned int offset;";
			}
				
			$lines[] = 		$handler->getAction();

			if($targetCount == 0) {
				// move the instruction pointer over this one
				$lines[] = 	"ip += sizeof($instr);";
			} else if($targetCount == 1) {
				// go to the jump target
				$lines[] = 	"ip = INSTR->instruction_pointer;";
			} else if($targetCount == 2) {
				// set the instruction pointer to pointer 1, if condition is true
				// otherwise update the next handler and set ip to pointer 2
				$lines[] = "if(condition) {";
				$lines[] = 		"ip = INSTR->instruction_pointer1;";
				$lines[] = "} else {";
				$lines[] = 		"handler = INSTR->next_handler2;";
				$lines[] = 		"ip = INSTR->instruction_pointer2;";
				$lines[] = "}";
			}  else if($targetCount > 2) {
				$lines[] = "handler = INSTR->branch_table[offset].next_handler;";
				$lines[] = "ip = INSTR->branch_table[offset].instruction_pointer;";
			}

			// go to the next instruction unless the function is returning
			if($targetCount != -1) {
				$lines[] = "handler(cxt, ip);";
			}
			$lines[] = $handler->getMacroUndefinitions();
			$lines[] = "}";
			$lines[] = "";
		}
		$lines[] = "#undef return";
		$lines[] = "";

		$lines[] = "void qb_main(qb_interpreter_context *__restrict cxt) {";
		$lines[] =		"if(setjmp(cxt->tail_call_exit) == 0) {";
		$lines[] = 			"register qb_tc_handler handler = ((qb_instruction *) cxt->instruction_pointer)->next_handler;";
		$lines[] = 			"register int8_t *__restrict ip = cxt->instruction_pointer + sizeof(qb_instruction);";		
		$lines[] =			"handler(cxt, ip);";
		$lines[] = 		"}";
		$lines[] = "}";
		
		$lines[] = "qb_op_handler *op_handlers[QB_OPCODE_COUNT] = {";
		foreach($this->handlers as $handler) {
			$name = $handler->getName();
			$lines[] =	"qb_tc_$name,";
		}
		$lines[] = "};";
		$this->writeCode($handle, $lines);
	}
			
	public function writeMainLoop($handle) {
		$this->currentIndentationLevel = 0;
		$this->writeCode($handle, "#if defined(USE_TAIL_CALL_INTERPRETER_LOOP)");
		$this->writeTailCallLoop($handle);
		$this->writeCode($handle, "#elif defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)");
		$this->writeComputedGotoLoop($handle);
		$this->writeCode($handle, "#else");
		$this->writeSwitchLoop($handle);
		$this->writeCode($handle, "#endif");
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
	}
	
	public function writeOpInfo($handle) {
		$this->currentIndentationLevel = 0;
		
		$prevHandler = null;
		$lines = array();
		$lines[] = "const qb_op_info global_op_info[] = {";
		foreach($this->handlers as $index => $handler) {		
			$flags = array();
			$relationToPrevious = Handler::compare($handler, $prevHandler);
			$targetCount = $handler->getJumpTargetCount();

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

			// set control flow flags
			if($targetCount == 1) {
				$flags[] = "QB_OP_JUMP";
			} else if($targetCount == 2) {
				$flags[] = "QB_OP_BRANCH";
			} else if($targetCount == -1) {
				$flags[] = "QB_OP_EXIT";
			} else if($targetCount > 2) {
				$flags[] = "QB_OP_BRANCH_TABLE";
			}
			
			if($handler->needsLineIdentifier()) {
				// the line number needs to be packed into the instruction (for error reporting)
				$flags[] = "QB_OP_NEED_LINE_IDENTIFIER";
			}
			if($handler->performsWrapAround()) {
				$flags[] = "QB_OP_PERFORM_WRAP_AROUND";
			}
			if($handler->needsInstructionStructure()) {
				$flags[] = "QB_OP_NEED_INSTRUCTION_STRUCT";
			}
			if(!$handler->isThreadSafe()) {
				$flags[] = "QB_OP_NOT_THREAD_SAFE";
			}
			$combined = ($flags) ? implode(" | ", $flags) : "0";
			
			$instr = $handler->getInstructionStructure();
			$length = ($instr) ? "sizeof($instr)" : 0;
			$format = $handler->getInstructionFormat();

			$name = $handler->getName();
			$lines[] = "// $name";
			$lines[] = "{	$combined, $length, \"$format\"	},";
			
			$prevHandler = $handler;
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
		$zend_opnames = $this->loadListing("zend_op_names.txt");
		$pbj_opnames = $this->loadListing("pixel_bender_op_names.txt");

		$this->writeCode($handle, "#ifdef HAVE_ZLIB");
		$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, true, true);
		$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, true, true);
		$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, true, true);
		$this->writeCode($handle, "#else");
		$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, false, true);
		$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, false, true);
		$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, false, true);
		$this->writeCode($handle, "#endif");
	}

	public function writeNativeCodeTables($handle) {
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

		// create function declaration table and reverse look-up table
		$functionDeclarations = $this->getFunctionDeclarations(true);
		$functionIndices = array();
		$functionRefCounts = array();
		foreach($functionDeclarations as $index => &$decl) {
			if(preg_match('/(\w+)\s*\(/', $decl, $m)) {
				$functionName = $m[1];
				$functionIndices[$functionName] = $index;
				$functionRefCounts[$index] = 0;
			}
		}
		unset($decl);
		
		// set up the function reference table
		$references = array();
		foreach($this->handlers as $handler) {
			$indices = array();
			$action = $handler->getAction();
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
								static $known_tokens = array('if', 'for', 'while', 'sizeof', 'return', 'QB_G', 'EG', 'SWAP_LE_I16', 'SWAP_LE_I32', 'SWAP_LE_I64', 'SWAP_BE_I16', 'SWAP_BE_I32', 'SWAP_BE_I64', 'EXPECTED', 'UNEXPECTED', 'USE_TSRM', 'zend_finite', 'zend_isinf', 'zend_isnan');
								if(!in_array($name, $known_tokens)) {
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
		
		$this->writeCode($handle, "#ifdef HAVE_ZLIB");
		$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_prototypes", $functionDeclarations, true, true);
		$this->writeCompressTable($handle, "compressed_table_native_references", $references, true, false);
		$this->writeCode($handle, "#else");
		$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_prototypes", $functionDeclarations, false, true);
		$this->writeCompressTable($handle, "compressed_table_native_references", $references, false, false);
		$this->writeCode($handle, "#endif");
	}

	protected function getSymbols($declarations) {
		$symbols = array();
		foreach($declarations as $decl) {
			if(preg_match('/^\s*(.*?)\s*(\w+)\s*\(([^)]*)\)/', $decl, $m)) {
				$modifiers = $m[1];
				$functionName = $m[2];
				$target = null;
				
				if(strpos($decl, "{") === false) {
					$target = $functionName;
				} else {
					// a function body could be generated inside the object file for the inline function
					// need to indicate that the symbol is known 
					$target = "NULL";
				}
				$symbols[$functionName] = $target;
			}
		}
		return $symbols;
	}
	
	public function writeNativeSymbolTable($handle) {
		$this->currentIndentationLevel = 0;
		
		// parse for declaration of helper functions 
		$functionDeclarations = $this->getFunctionDeclarations(false);
		
		// figure out what the symbols are
		$symbols = $this->getSymbols($functionDeclarations);

		// load lists of function prototypes functions
		$commonSymbols = $this->getSymbols($this->loadListing("function_prototypes.txt"));
		$gccSymbols = $this->getSymbols($this->loadListing("function_prototypes_gcc.txt"));
		$msvcSymbols = $this->getSymbols($this->loadListing("function_prototypes_msvc.txt"));

		// load lists of intrinsic functions
		$msvcIntrinsics = $this->loadListing("intrinsic_functions_msvc.txt");
		$gccIntrinsics = $this->loadListing("intrinsic_functions_gcc.txt");
		
		// these functions cannot be declared in VC11
		$vc11Intrinsics =  $this->loadListing("intrinsic_functions_msvc11.txt");

		$msvcFunctionPointers = array("floor", "ceil");

		$lines = array();
		$lines[] = "qb_native_symbol global_native_symbols[] = {";
		foreach($symbols as $name => $symbol) {
			if($symbol == "NULL") {
				$lines[] = "{	\"$name\",	$symbol,	0,	QB_NATIVE_SYMBOL_INLINE_FUNCTION	},";
			} else {
				$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
			}
		}
		
		foreach($commonSymbols as $name => $symbol) {
			if(in_array($name, $vc11Intrinsics)) {
				$lines[] = "#if _MSC_VER > 1700";
				$lines[] = "{	\"$name\",	$symbol,	0,	QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION	},";
				$lines[] = "#else";
				$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
				$lines[] = "#endif";
			} else if(in_array($name, $msvcFunctionPointers)) {
				$lines[] = "#if defined(_MSC_VER)";
				$lines[] = "{	\"$name\",	NULL,	0,	0	},";
				$lines[] = "#else";
				$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
				$lines[] = "#endif";
			} else {
				$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
			} 
		}

		$lines[] = "#if defined(_MSC_VER)";
		foreach($msvcSymbols as $name => $symbol) {
			$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
		}
		foreach($msvcIntrinsics as $name) {
			$lines[] = "{	\"$name\",	NULL,	0,	QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION	},";
		}
		$lines[] = "#elif defined(__GNUC__)";
		foreach($gccSymbols as $name => $symbol) {
			$lines[] = "{	\"$name\",	$symbol,	0,	0	},";
		}
		foreach($gccIntrinsics as $name) {
			$lines[] = "{	\"$name\",	NULL,	0,	QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION	},";
		}
		$lines[] = "#endif";
		$lines[] = "};";
		$lines[] = "";
		$lines[] = "uint32_t global_native_symbol_count = sizeof(global_native_symbols) / sizeof(qb_native_symbol);";
		$this->writeCode($handle, $lines);
	}

	public function writeNativeDebugStub($handle) {
		$lines = array();
		$lines[] = "#if NATIVE_COMPILE_ENABLED && ZEND_DEBUG";
		$lines[] = "#	include \"qb_native_proc_debug.c\"";
		$lines[] = "#	ifdef HAVE_NATIVE_PROC_RECORDS";
		$lines[] = "qb_native_proc_record *native_proc_table = native_proc_records;";
		$lines[] = "uint32_t native_proc_table_size = sizeof(native_proc_records) / sizeof(qb_native_proc_record);";
		$lines[] = "#	else";
		$lines[] = "qb_native_proc_record *native_proc_table = NULL;";
		$lines[] = "uint32_t native_proc_table_size = 0;";
		$lines[] = "#	endif";
		$lines[] = "#endif";
		$this->writeCode($handle, $lines);
	}
	
	protected function loadListing($filename) {
		$folder = dirname(__FILE__);
		$path = "$folder/listings/$filename";
		$lines = file($path, FILE_IGNORE_NEW_LINES);
		$filteredLines = array();
		foreach($lines as $line) {
			$line = preg_replace('/\/\/.*/', '', $line);
			$line = trim($line);
			if($line) {
				$filteredLines[] = $line;
			}
		}
		return $filteredLines;
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
	
	protected function parseFunctionDeclarations() {
	}

	protected function getFunctionDeclarations($addPrototypes) {
		$functionDeclarations = array();
		
		// add the generated function first
		$functionDefinitions = $this->getFunctionDefinitions();
		
		// then prepend the list with ones that aren't generated
		if($addPrototypes) {
			$common = $this->loadListing("function_prototypes.txt");
			$gcc = $this->loadListing("function_prototypes_gcc.txt");
			$msvc = $this->loadListing("function_prototypes_msvc.txt");
			foreach(array_merge($common, $gcc, $msvc) as $line) {
				array_unshift($functionDefinitions, array($line));
			}
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

	protected function writeCode($handle, $lines) {
		if(!is_array($lines)) {
			$lines = array($lines);
		}
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
		fwrite($handle, "const char {$name}[{$len}] = ");
		for($i = 0, $j = 0, $len = strlen($tableData); $i < $len; $i++, $j--) {
			if($j == 0) {
				if($i > 0) {
					// close out the previous line
					fwrite($handle, "\"");
				}
				// start on a new line
				fwrite($handle, "\n\"");
				$j = 80;
			}
			fwrite($handle, sprintf("\x%02X", ord($tableData[$i])));
		}
		fwrite($handle, "\";\n");
	}
	
	protected function addHandlers() {
		foreach($this->elementTypes as $elementType) {
			$this->addFlowControlHandlers($elementType);
			$this->addIncrementDecrementHandlers($elementType);
			$this->addBoundCheckingHandlers($elementType);
			$this->addAssignmentHandlers($elementType);
			$this->addArithmeticHandlers($elementType);
			$this->addBitwiseHandlers($elementType);
			$this->addComparisonHandlers($elementType);
			$this->addLogicalHandlers($elementType);
			$this->addMathHandlers($elementType);
			$this->addArrayHandlers($elementType);
			$this->addSamplingHandlers($elementType);
			$this->addMatrixHandlers($elementType);
			$this->addComplexNumberHandlers($elementType);
		}
		foreach($this->elementTypes as $elementType) {
			$this->addCastHandlers($elementType);
			$this->addScatterGatherHandlers($elementType);
			$this->addUnsetHandlers($elementType);
			$this->addStringHandlers($elementType);
		}
		$this->addRuntimeValidationHandlers();
		$this->addMiscHandlers();
	}
	
	protected function addBoundCheckingHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if($elementType == "U32") {
			$this->handlers[] = new GuardIndex("IDX_GUARD", $elementType);
			$this->handlers[] = new GuardIndexAdd("IDX_GUARD_ADD", $elementType);
			$this->handlers[] = new GuardIndexMultiply("IDX_GUARD_MUL", $elementType);
			$this->handlers[] = new GuardIndexMultiplyAdd("IDX_GUARD_MAC", $elementType);
			
			$this->handlers[] = new GuardSize("SZ_GUARD", $elementType);
			$this->handlers[] = new GuardSizeExact("SZ_GUARD_EX", $elementType);
			
			$this->handlers[] = new GuardExtent("EXT_GUARD", $elementType);
			$this->handlers[] = new GuardExtentMultiply("EXT_GUARD_MUL", $elementType);
			$this->handlers[] = new GuardExtentSubtract("EXT_GUARD_SUB", $elementType);
			$this->handlers[] = new GuardExtentSubtractMultiply("EXT_GUARD_SUB_MUL", $elementType);
			
			$this->handlers[] = new CheckIndexAdd("IDX_CHECK_ADD", $elementType);
			$this->handlers[] = new CheckIndexMultiply("IDX_CHECK_MUL", $elementType);
			$this->handlers[] = new CheckIndexMultiplyAdd("IDX_CHECK_MAC", $elementType);
			
			$this->handlers[] = new AccommodateIndex("IDX_ACCOM", $elementType);
			$this->handlers[] = new AccommodateIndexMultiply("IDX_ACCOM_MUL", $elementType);
			$this->handlers[] = new AccommodatePush("IDX_ACCOM_PUSH", $elementType);
			
			$this->handlers[] = new AccommodateSize("SZ_ACCOM", $elementType);
			$this->handlers[] = new AccommodateSizeUpdateDimension("SZ_ACCOM_DIM", $elementType);
			for($i = 2; $i <= 8; $i++) {
				$this->handlers[] = new AccommodateSizeCopyDimension("SZ_ACCOM_DIM$i", $elementType, $i);
			}
			
			$this->handlers[] = new ChooseSizeLargerOfTwo("SZ_CHOOSE_2", $elementType);
			$this->handlers[] = new ChooseSizeLargerOfTwoTopLevel("SZ_CHOOSE_2_TOP", $elementType);
			$this->handlers[] = new ChooseSizeLargestOfThree("SZ_CHOOSE_3", $elementType);
			$this->handlers[] = new ChooseSizeLargestOfThreeTopLevel("SZ_CHOOSE_3_TOP", $elementType);
		}
		
		if(!$unsigned) {
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new PredicateBooleanCast("CBOOL", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addArithmeticHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Add("ADD", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Multiply("MUL", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$width = (int) substr($elementType, 1);
			if($width >= 32) {
				$this->handlers[] = new MultiplyAccumulate("MAC", $elementType, $addressMode);
			}
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new Subtract("SUB", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Divide("DIV", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new Modulo("MOD", $elementType, $addressMode);
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
	
	protected function addScatterGatherHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			for($i = 2; $i <= 8; $i++) {
				$this->handlers[] = new Gather("GATH", $elementTypeNoSign, $i);
			}
			for($i = 2; $i <= 8; $i++) {
				$this->handlers[] = new Scatter("SCAT", $elementTypeNoSign, $i);
			}
		}
	}
	
	protected function addUnsetHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			$this->handlers[] = new ClearScalar("CLR_SCA", $elementTypeNoSign);
			$this->handlers[] = new ClearArrayResize("CLR_ARR_RSZ", $elementTypeNoSign);
			$this->handlers[] = new ClearArrayResizeUpdateDimension("CLR_ARR_RSZ_DIM", $elementTypeNoSign);
			$this->handlers[] = new ClearElementResize("CLR_ELE_RSZ", $elementTypeNoSign);
			$this->handlers[] = new ClearElementResizeUpdateDimension("CLR_ELE_RSZ_DIM", $elementTypeNoSign);

			$this->handlers[] = new PredicateClearArray("CCLR_ARR", $elementTypeNoSign);
			$this->handlers[] = new PredicateClearElement("CCLR_ELE", $elementTypeNoSign);
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
			$this->handlers[] = new LessThanEqual("LE", $elementType, $addressMode);
		}
		if(!$unsigned) {
			$this->handlers[] = new SetEqual("EQ_SET", $elementTypeNoSign);
			$this->handlers[] = new SetNotEqual("NE_SET", $elementTypeNoSign);
		}
		$this->handlers[] = new SetLessThan("LT_SET", $elementType);
		$this->handlers[] = new SetLessThanEqual("LE_SET", $elementType);
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
			foreach($this->scalarAddressModes as $addressMode) {
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
				$this->handlers[] = new Rint("RINT", $elementType, $addressMode);
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
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new RoundToPrecision("ROUND", $elementType, $addressMode);
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
		if($elementType == "U32") {
			$this->handlers[] = new NOP("NOP");
			$this->handlers[] = new Jump("JMP");
			$this->handlers[] = new Leave("RET");
			$this->handlers[] = new Interrupt("INTR");
			$this->handlers[] = new Terminate("EXIT", "I32");
			$this->handlers[] = new FunctionCall("FCALL");
			$this->handlers[] = new StaticInitializationEnd("END_STATIC");
			$this->handlers[] = new Fork("FORK", "U32");
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new ForkIdentifier("FORK_ID", "U32", $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new ForkCount("FORK_CNT", "U32", $addressMode);
			}
			$this->handlers[] = new Resume("RESUME");
			$this->handlers[] = new Spoon("SPOON");
		}
		$branchHandlers = array();
		if($elementType == "U32") {
			$this->handlers[] = new IncrementBranchOnGreaterThan("INC_IF_GT", $elementType, "SCA");
		}
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
			$this->handlers[] = new BranchOnLessThanEqual("IF_LE", $elementType, $addressMode);
		}
		
		$branch_table_sizes = array(8, 16, 32, 64, 128, 256, 512, 1024);

		if(!$unsigned) {
			foreach($branch_table_sizes as $size) {
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new BranchTable("SWITCH$size", $elementTypeNoSign, $addressMode, $size);
				}
			}
		}
	}

	protected function addStringHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$float = preg_match('/^F/', $elementType);
		if($elementType == "U08") {
			$this->handlers[] = new PrintString("PRN_STR", $elementType);
			$this->handlers[] = new AppendString("APP_STR", $elementType);
			$this->handlers[] = new Flush("FLUSH");
			
		}
		if($elementType == "U16" || $elementType == "U32") {
			if($elementType == "U16") {
				$this->handlers[] = new UTF8DecodeCount("SZ_UTF8_DEC", $elementType);
			}			
			$this->handlers[] = new UTF8Decode("UTF8_DEC", $elementType);
			$this->handlers[] = new UTF8EncodeCount("SZ_UTF8_ENC", $elementType);
			$this->handlers[] = new UTF8Encode("UTF8_ENC", $elementType);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new PrintVariable("PRN", $elementType, $addressMode);
		}
		$this->handlers[] = new PrintMultidimensionalVariable("PRN_DIM", $elementType);
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new AppendVariable("APP_VAR", $elementType, $addressMode);
		}
		$this->handlers[] = new AppendMultidimensionalVariable("APP_VAR_DIM", $elementType);
		
		if(!$unsigned && $elementTypeNoSign != "I08") {
			foreach($this->scalarAddressModes as $addressMode) {		
				$this->handlers[] = new PackLittleEndian("PACK_LE", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new PackBigEndian("PACK_BE", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new UnpackLittleEndian("UNPACK_LE", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new UnpackBigEndian("UNPACK_BE", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ConvertFromString("CONV_STR", $elementType, $addressMode);
		}
	}
	
	protected function addArrayHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$this->handlers[] = new Sort("SORT", $elementType);
		$this->handlers[] = new ReverseSort("RSORT", $elementType);
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ArrayMin("AMIN", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ArrayMax("AMAX", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ArrayProduct("APROD", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new ArraySum("ASUM", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new Range("RANGE", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new RangeCount("SZ_RANGE", $elementType, $addressMode);
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ArraySearch("AFIND", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new ArrayFill("AFILL", $elementTypeNoSign, $addressMode);
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
			$this->handlers[] = new ArrayUniqueCount("SZ_AUNIQ", $elementTypeNoSign);
			$this->handlers[] = new ArrayColumn("ACOL", $elementTypeNoSign);
			$this->handlers[] = new ArrayDifference("ADIFF", $elementTypeNoSign);
			$this->handlers[] = new ArrayDifferenceCount("SZ_ADIFF", $elementTypeNoSign);
			$this->handlers[] = new ArrayIntersect("AISECT", $elementTypeNoSign);
			$this->handlers[] = new ArrayIntersectCount("SZ_AISECT", $elementTypeNoSign);
			$this->handlers[] = new Shuffle("SHUFFLE", $elementTypeNoSign);
			
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new ArraySlice("ASLICE", $elementTypeNoSign, $addressMode);
			}
			if($elementTypeNoSign == "I32") {
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new ArraySliceCount("SZ_ASLICE", $elementTypeNoSign, $addressMode);
				}
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new ArrayReplace("AREPLACE", $elementTypeNoSign, $addressMode);
			}
			
			for($i = 1; $i <= 8; $i++) {
				$this->handlers[] = new ArrayResize("ARESIZE$i", $elementTypeNoSign, $i);
			}
			
			$this->handlers[] = new ArrayPad("APAD", $elementTypeNoSign);
		}
		if($elementType == 'U32') {
			$this->handlers[] = new ArrayPadCount("SZ_APAD", 'U32');
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
				$this->handlers[] = new SampleNearest("SAMPLE_NN", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleNearest("SAMPLE_NN", $elementType, $addressMode, 1, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 4, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 3, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 2, ($addressMode == "ARR"));
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new SampleBilinear("SAMPLE_BL", $elementType, $addressMode, 1, ($addressMode == "ARR"));
			}
			$this->handlers[] = new AlphaBlend("BLEND", $elementType, 2);
			$this->handlers[] = new AlphaBlend("BLEND", $elementType, 2, true);
			$this->handlers[] = new AlphaBlend("BLEND", $elementType, 4);
			$this->handlers[] = new AlphaBlend("BLEND", $elementType, 4, true);
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
	
	protected function addRuntimeValidationHandlers() {
		$this->handlers[] = new ValidateMatrixByMatrix("MUL_MM_GUARD", "U32");
		$this->handlers[] = new ValidateMatrixByVector("MUL_MV_GUARD", "U32");
		$this->handlers[] = new ValidateVectorByMatrix("MUL_VM_GUARD", "U32");
		$this->handlers[] = new ValidateSquareMatrix("SM_GUARD", "U32");
		$this->handlers[] = new ValidateTransform("TRAN_GUARD", "U32");
		$this->handlers[] = new Validate2DCrossProduct("CROSS_2X_GUARD", "U32");
		$this->handlers[] = new Validate3DCrossProduct("CROSS_3X_GUARD", "U32");
		$this->handlers[] = new Validate4DCrossProduct("CROSS_4X_GUARD", "U32");
		$this->handlers[] = new ValidateVectorWidths("VV_GUARD", "U32");
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
	
	protected function addMiscHandlers() {
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new GetTime("TIME", "F64", $addressMode);
		}
		$this->handlers[] = new ExtensionOp("EXT", "U32");
		$this->handlers[] = new SynchronizeShadowVariable("DBG_SYNC", "U32");
	}
}

?>
