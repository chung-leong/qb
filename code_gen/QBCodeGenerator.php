<?php

class QBCodeGenerator {
	protected $handlers = array();
	protected $elementTypes = array("S32", "U32", "S08", "U08", "S16", "U16", "S64", "U64", "F32", "F64");
	protected $floatTypes = array("F32", "F64");
	protected $addressModes = array("VAR", "ELC", "ELV", "ARR");
	protected $scalarAddressModes = array("VAR", "ELC", "ELV");
	
	protected $currentIndentationLevel;
	
	public function __construct() {
		if(!$this->handlers) {
			$this->addHandlers();
		}
	}

	public function generate($handle, $compiler, $output) {
		QBHandler::setCompiler($compiler);
		$this->currentIndentationLevel = 0;
		
		if($output == "HANDLERS") {
			$helperExists = array();
			foreach($this->handlers as $handler) {
				$functions = $handler->getHelperFunctions();
				if($functions) {
					foreach($functions as $lines) {
						$line1 = $lines[0];
						if(!isset($helperExists[$line1])) {
							$lines[] = "";
							$this->writeCode($handle, $lines);
							$helperExists[$line1] = true;
						}
					}
				}
			}
		
			$lines = array();
			$lines[] = "static void ZEND_FASTCALL qb_run(qb_interpreter_context *__restrict cxt) {";
			$lines[] =		QBHandler::getMacroDefinitions();
			$lines[] =		"";
			$lines[] = 		"if(cxt) {";
			$lines[] = 			"register void *__restrict op_handler;";
			$lines[] = 			"register int8_t *__restrict instruction_pointer;";
			$lines[] = 			"int8_t *__restrict segments[MAX_SEGMENT_COUNT];";
			$lines[] = 			"int8_t *__restrict segment0;";
			$lines[] =			"int32_t segment_expandable[MAX_SEGMENT_COUNT];";			 
			$lines[] = 			"uint32_t segment_element_counts[MAX_SEGMENT_COUNT];";
			$lines[] = 			"char sprintf_buffer[64];";
			$lines[] = 			"uint32_t string_length;";
			$lines[] = 			"uint32_t vector_count, matrix1_count, matrix2_count, mmult_res_count;";
			
			if($compiler == "MSVC") {
				$lines[] = 		"zend_bool *windows_timed_out_pointer = cxt->windows_timed_out_pointer;";
			}
			$lines[] =			"USE_TSRM";
			$lines[] = 			"";
			$lines[] = 			"{";
			$lines[] = 				"uint32_t i;";
			$lines[] = 				"instruction_pointer = cxt->function->instructions;";
			$lines[] = 				"op_handler = *((void **) instruction_pointer);";
			$lines[] = 				"instruction_pointer += sizeof(void *);";
			$lines[] =				"// copy values from cxt onto the stack so they can be accessed without two second deferences";
			$lines[] = 				"for(i = 0; i < cxt->storage->segment_count; i++) {";
			$lines[] = 					"qb_memory_segment *segment = &cxt->storage->segments[i];";
			$lines[] = 					"segments[i] = segment->memory;";
			$lines[] = 					"segment_element_counts[i] = *segment->array_size_pointer;";
			$lines[] = 					"segment_expandable[i] = (segment->flags & QB_SEGMENT_EXPANDABLE);";
			$lines[] =					"// set the pointers in the segment structure to local variables here so we can update";
			$lines[] =					"// them as well when we expand the segment";
			$lines[] =					"segment->stack_ref_memory = &segments[i];";
			$lines[] =					"segment->stack_ref_element_count = &segment_element_counts[i];";
			$lines[] = 				"}";
			$lines[] =				"// store pointer to segment 0 in a separate variable to enable better optimization";
			$lines[] =				"// since segment 0 and 1 will never be enlarged, we don't have to worry about it changing";
			$lines[] =				"segment0 = segments[0];";			
			if($compiler == "GCC") {
				$lines[] = 			"goto *op_handler;";
			}
			$lines[] = 			"}";
			$lines[] = "";
			if($compiler == "MSVC") {
				// Visual C doesn't support computed goto so we have to use a giant switch() statement instead
				$lines[] = 		"do {";
				$lines[] = 			"switch((int) op_handler) {";
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
			$lines[] = 			"label_exit:";
			$lines[] = 			"{";
			$lines[] = 				"uint32_t i;";
			$lines[] =				"// point the stack_ref pointer back to variables in the structure";
			$lines[] = 				"for(i = 0; i < cxt->storage->segment_count; i++) {";
			$lines[] = 					"qb_memory_segment *segment = &cxt->storage->segments[i];";
			$lines[] =					"segment->stack_ref_memory = &segment->memory;";
			$lines[] =					"segment->stack_ref_element_count = &segment->element_count;";
			$lines[] = 				"}";
			$lines[] = 				"return;";
			$lines[] = 			"}";
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
			$lines[] =		QBHandler::getMacroUndefinitions();
			$lines[] = 	"}";
			$lines[] = 	"";
			if($compiler == "GCC") {
				$lines[] = 	"void *op_handlers[QB_OPCODE_COUNT];";
			}
			$this->writeCode($handle, $lines);
		} else if($output == "DECLARATIONS") {
			$this->writeCode($handle, array("#pragma pack(push,1)", ""));
			$decls = QBHandler::getTypeDeclarations();
			foreach($decls as $lines) {
				$lines[] = "";
				$this->writeCode($handle, $lines);
			}
			$this->writeCode($handle, array("#pragma pack(pop)", ""));

			$lines = array();
			$lines[] = "extern void *op_handlers[];";
			$lines[] = "";
			$this->writeCode($handle, $lines);
		} else if($output == "CODES") {
			$lines = array();
			$lines[] = "enum {";
			foreach($this->handlers as $handler) {
				$name = $handler->getName();
				if(preg_match("/(\w+)_VAR$/", $name, $m)) {
					$baseName = $m[1];
					$lines[] = "QB_$baseName,";
					$lines[] = "QB_$name = QB_$baseName,";
				} else {
					$lines[] = "QB_$name,";
				}
			}
			$lines[] = "QB_OPCODE_COUNT";
			$lines[] = "};";
			$lines[] = "";
			$lines[] = "enum {";
			foreach($this->addressModes as $index => $addressMode) {
				$lines[] =	"QB_OPCODE_OFFSET_$addressMode = $index,";
			}
			$lines[] = "};";
			$this->writeCode($handle, $lines);
		} else if($output == "FLAGS") {
			$lines = array();
			$lines[] = "uint32_t global_op_flags[] = {";
			foreach($this->handlers as $handler) {
				$flags = $handler->getOpFlags();
				$flags = ($flags) ? implode(" | ", $flags) : "0";
				$name = $handler->getName();
				$lines[] = "// $name";
				$lines[] = "$flags,";
			}
			$lines[] = "};";
			$lines[] = "";
			$lines[] = "uint32_t global_operand_flags[] = {";
			foreach($this->handlers as $handler) {
				$name = $handler->getName();
				$list = $handler->getOperandFlags();
				if(count($list) > 8) {
					$count = count($list);
					throw new Exception("$name has too many operands ($count)! We only got 32 bits to store the operand flags!");
				}
				$flags = array();
				foreach($list as $index => $operandFlags) {
					$shift = $index * 4;
					$flags[] = "($operandFlags << $shift)";
				}
				$flags = ($flags) ? implode(" | ", $flags) : "0";
				$lines[] = "// $name";
				$lines[] = "$flags,";
			}
			$lines[] = "};";
			$lines[] = "";
			
			$this->writeCode($handle, $lines);
		} else if($output == "NAMES") {
			$opnames = array();
			foreach($this->handlers as $handler) {
				$opnames[] = $handler->getName();
			}
			$folder = dirname(__FILE__);
			$zend_opnames = file("$folder/zend_op_names.txt", FILE_IGNORE_NEW_LINES);
			$pbj_opnames = file("$folder/pixel_bender_op_names.txt", FILE_IGNORE_NEW_LINES);

			fwrite($handle, "#ifdef HAVE_ZLIB\n");
			$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, true, true);
			$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, true, true);
			$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, true, true);
			fwrite($handle, "#else\n");
			$this->writeCompressTable($handle, "compressed_table_op_names", $opnames, false, true);
			$this->writeCompressTable($handle, "compressed_table_zend_op_names", $zend_opnames, false, true);
			$this->writeCompressTable($handle, "compressed_table_pbj_op_names", $pbj_opnames, false, true);
			fwrite($handle, "#endif\n");
		} else if($output == "NATIVE CODE") {
			$actions = array();
			foreach($this->handlers as $index => $handler) {
				try {
					$lines = $handler->getAction();
					if($lines) {
						$actions[$index] = $this->formatCode($lines);
					}
				} catch(Exception $e) {
				}
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
				}
			}
			
			// scan for function prototypes
			$functionDecls = array();
			$processed = array();
			foreach($this->handlers as $handler) {
				$functions = $handler->getHelperFunctions();
				if($functions) {
					foreach($functions as $lines) {
						$line1 = $lines[0];
						if(!isset($processed[$line1])) {
							$decl = $this->parseFunctionDeclaration($line1);
							if($decl) {
								$functionDecls[$decl->name] = $decl;
								$processed[$line1] = true;
							}
						}
					}
				}
			}
			$folder = dirname(__FILE__);
			$list = file("$folder/function_prototypes.txt", FILE_IGNORE_NEW_LINES);
			foreach($list as $line) {
				$decl = $this->parseFunctionDeclaration($line);
				$functionDecls[$decl->name] = $decl;
			}						
			ksort($functionDecls);
			
			// set up the prototype table
			$prototypeIndices = array();
			$prototypes = array();
			$index = 0;
			foreach($functionDecls as $decl) {
				$parameterDecls = implode(", ", $decl->parameterDecls);
				$prototypes[] = "$decl->returnType $decl->name($parameterDecls);";
				$prototypeIndices[$decl->name] = $index++;
			}
			
			// set up the function reference table
			$references = array();
			foreach($this->handlers as $handler) {
				$calls = $handler->getFunctionsUsed();
				$indices = array();
				if($calls) {
					foreach($calls as $name) {
						if(!preg_match('/^__builtin/', $name)) {
							$index = $prototypeIndices[$name];
							if($index !== null) {
								$indices[] = $index;
							} else {
								throw new Exception("Missing function prototype for $name");
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
			
			fwrite($handle, "#ifdef HAVE_ZLIB\n");
			$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, true, true);
			$this->writeCompressTable($handle, "compressed_table_native_result_size_calculations", $resultSizeCalculations, true, true);
			$this->writeCompressTable($handle, "compressed_table_native_result_size_possibilities", $resultSizePossibilities, true, true);
			$this->writeCompressTable($handle, "compressed_table_native_prototypes", $prototypes, true, true);
			$this->writeCompressTable($handle, "compressed_table_native_references", $references, true, false);
			fwrite($handle, "#else\n");
			$this->writeCompressTable($handle, "compressed_table_native_actions", $actions, false, true);
			$this->writeCompressTable($handle, "compressed_table_native_result_size_calculations", $resultSizeCalculations, false, true);
			$this->writeCompressTable($handle, "compressed_table_native_result_size_possibilities", $resultSizePossibilities, false, true);
			$this->writeCompressTable($handle, "compressed_table_native_prototypes", $prototypes, false, true);
			$this->writeCompressTable($handle, "compressed_table_native_references", $references, false, false);
			fwrite($handle, "#endif\n");
		} else if($output == "NATIVE SYMBOLS") {
			// parse for declaration of helper functions 
			$functionDecls = array();
			$needWrapper = array();
			$processed = array();
			foreach($this->handlers as $handler) {
				$functions = $handler->getHelperFunctions();
				if($functions) {
					foreach($functions as $lines) {
						$line1 = $lines[0];
						if(!isset($processed[$line1])) {
							$decl = $this->parseFunctionDeclaration($line1);
							if($decl) {
								$functionDecls[$decl->name] = $decl;
								$processed[$line1] = true;
							}
						}
					}
				}
			}
			// add other functions 
			$folder = dirname(__FILE__);			
			$list = file("$folder/function_prototypes.txt", FILE_IGNORE_NEW_LINES);
			foreach($list as $line) {
				$decl = $this->parseFunctionDeclaration($line);
				if($decl) {
					$functionDecls[$decl->name] = $decl;
				}
			}
			ksort($functionDecls);
			
			// print out wrappers
			foreach($functionDecls as $decl) {
				if($decl->fastcall || $decl->static) {
					$parameterDecls = implode(", ", $decl->parameterDecls);
					$parameters = implode(", ", $decl->parameters);
					$fcall = "{$decl->name}($parameters)";
					fwrite($handle, "{$decl->returnType} {$decl->name}_wrapper($parameterDecls) {\n");
					if($decl->returnType != 'void') {
						fwrite($handle, "	return $fcall;\n");
					} else {
						fwrite($handle, "	$fcall;\n");
					}
					fwrite($handle, "}\n\n");
				}
			}
		
			// make sure that functions called by the op handlers are accounted for
			$functionsInUse = array();
			foreach($this->handlers as $index => $handler) {
				$addition = array_diff($handler->getFunctionsUsed(), $functionsInUse);
				foreach($addition as $name) {
					if(!preg_match('/^__builtin/', $name)) {						
						$functionsInUse[] = $name;
						
						if(!isset($functionDecls[$name])) {
							throw new Exception("Missing declaration for '$name'");						
						}
					}
				}
			}
			
			fwrite($handle, "qb_native_symbol global_native_symbols[] = {\n");
			foreach($functionDecls as $name => $decl) {
				if($decl->fastcall || $decl->static) {
					$symbol = "{$name}_wrapper";
				} else {
					$symbol = $name;
				}
				fwrite($handle, "	{	0,	\"$name\",	$symbol	},\n");
			}
			fwrite($handle, "};\n");
		}
	}

	protected function parseFunctionDeclaration($line) {
		if(preg_match('/^\s*(.*?)\s*(\w+)\s*\(([^)]*)\)\s*[{;]\s*$/', $line, $m)) {
			$name = $m[2];
			$wrapper = array();
			$returnType = $m[1]; 
			$parameterDecls = preg_split('/\s*,\s+/', $m[3]);
			$parameters = array();
			foreach($parameterDecls as $parameterDecl) {
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
			$func = new stdClass;
			$func->name = $name;
			if(preg_match('/\bstatic\b/', $returnType)) {
				$returnType = preg_replace('/\bstatic\b/', "", $returnType);
				$func->static = true;
			} else {
				$func->static = false;
			}
			if(preg_match('/\bZEND_FASTCALL\b/', $returnType)) {
				$returnType = preg_replace('/\bZEND_FASTCALL\b/', "", $returnType);
				$func->fastcall = true;
			} else {
				$func->fastcall = false;
			}
			$func->returnType = trim($returnType);
			$func->parameterDecls = $parameterDecls;
			$func->parameters = $parameters;
			return $func;
		}
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
					$this->currentIndentationLevel -= substr_count($line, "}");
					if(!preg_match('/^#/', $line)) {
						$line = str_repeat("\t", $this->currentIndentationLevel) . $line;
					}
					$code .= "$line\n";
					if($this->currentIndentationLevel < 0) {
						throw new Exception("There's a problem with the code indentation\n");
					}
					$this->currentIndentationLevel += substr_count($line, "{");
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
			$this->addArithmeticHandlers($elementType);
			$this->addAssignmentHandlers($elementType);
			$this->addIncrementDecrementHandlers($elementType);
			$this->addComparisonHandlers($elementType);
			$this->addBitwiseHandlers($elementType);
			$this->addLogicalHandlers($elementType);
			$this->addCastHandlers($elementType);
			$this->addMathHandlers($elementType);
			$this->addStringHandlers($elementType);
			$this->addArrayHandlers($elementType);
			$this->addSamplingHandlers($elementType);
			$this->addMatrixHandlers($elementType);
		}
		$this->addDebugHandlers();
	}

	protected function addArithmeticHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBAddHandler("ADD", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSubtractHandler("SUB", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBMultiplyHandler("MUL", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBDivideHandler("DIV", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBModuloHandler("MOD", $elementType, $addressMode);
		}
		if($float) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBFlooredDivisionModuloHandler("MOD_FLR", $elementType, $addressMode);
			}		
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBNegateHandler("NEG", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addAssignmentHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBCopyHandler("MOV", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addIncrementDecrementHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBIncrementHandler("INC", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDecrementHandler("DEC", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addComparisonHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBEqualHandler("EQ", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBNotEqualHandler("NE", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBLessThanHandler("LT", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBLessThanOrEqualHandler("LE", $elementType, $addressMode);
		}
		if(!$unsigned) {
			$this->handlers[] = new QBEqualVectorHandler("EQ_SET", $elementTypeNoSign);
			$this->handlers[] = new QBNotEqualVectorHandler("NE_SET", $elementTypeNoSign);
		}
		$this->handlers[] = new QBLessThanVectorHandler("LT_SET", $elementType);
		$this->handlers[] = new QBLessThanOrEqualVectorHandler("LE_SET", $elementType);
		if($elementTypeNoSign == "I32") {
			$this->handlers[] = new QBNotVectorHandler("NOT_SET", $elementTypeNoSign);
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBAnyHandler("ANY", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBAllHandler("ALL", $elementTypeNoSign, $addressMode);
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
					$this->handlers[] = new QBBitwiseAndHandler("BW_AND", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new QBBitwiseOrHandler("BW_OR", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new QBBitwiseXorHandler("BW_XOR", $elementTypeNoSign, $addressMode);
				}
				foreach($this->addressModes as $addressMode) {
					$this->handlers[] = new QBBitwiseNotHandler("BW_NOT", $elementTypeNoSign, $addressMode);
				}
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBShiftLeftHandler("SHL", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBShiftRightHandler("SHR", $elementType, $addressMode);
			}
		}
	}

	protected function addLogicalHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!$unsigned) {
			if($elementTypeNoSign == "I32") {
				$this->handlers[] = new QBLogicalAndHandler("AND", $elementTypeNoSign, "VAR");
				$this->handlers[] = new QBLogicalOrHandler("OR", $elementTypeNoSign, "VAR");
				$this->handlers[] = new QBLogicalXorHandler("XOR", $elementTypeNoSign, "VAR");
				$this->handlers[] = new QBLogicalNotHandler("NOT", $elementTypeNoSign, "VAR");
			}
			$this->handlers[] = new QBIssetHandler("ISSET", $elementTypeNoSign, "ELV");
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBUnsetHandler("UNSET", $elementTypeNoSign, $addressMode);
			}
		}
	}

	protected function addCastHandlers($elementType) {
		foreach($this->elementTypes as $destElementType) {
			$elementUnsigned = preg_match('/^U/', $elementType);
			$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
			$destUnsigned = preg_match('/^U/', $destElementType);
			$destTypeNoSign = preg_replace('/^S/', "I", $destElementType);
			if($elementType != $destElementType && $elementTypeNoSign != $destTypeNoSign) {
				$float = preg_match('/^F/', $elementType) || preg_match('/^F/', $destElementType);
				$promotion = (int) substr($elementType, 1) < (int) substr($destTypeNoSign, 1);
				if($float) {
					// sign matters when floating points are involved
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new QBCastHandler("MOV", $elementType, $destElementType, $addressMode);
					}
				} else if($promotion && !$destUnsigned) {
					// sign matters when promoting from lower-rank integer type to higher-rank type
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new QBCastHandler("MOV", $elementType, $destTypeNoSign, $addressMode);
					}
				} else if(!$elementUnsigned && !$destUnsigned) {
					// sign doesn't matter
					foreach($this->addressModes as $addressMode) {
						$this->handlers[] = new QBCastHandler("MOV", $elementTypeNoSign, $destTypeNoSign, $addressMode);
					}
				}
			}
		}
	}

	protected function addMathHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		$elementUnsigned = preg_match('/^U/', $elementType);
		if(!$elementUnsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBAbsHandler("ABS", $elementType, $addressMode);
			}
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBMinHandler("MIN", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBMaxHandler("MAX", $elementType, $addressMode);
		}
		if($float) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSinHandler("SIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBASinHandler("ASIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBCosHandler("COS", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBACosHandler("ACOS", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBTanHandler("TAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBATanHandler("ATAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBATan2Handler("ATAN2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSinhHandler("SINH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBASinhHandler("ASINH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBCoshHandler("COSH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBACoshHandler("ACOSH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBTanhHandler("TANH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBATanhHandler("ATANH", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBCeilHandler("CEIL", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBFloorHandler("FLOOR", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBRoundHandler("ROUND", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLogHandler("LOG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLog1PHandler("LOG1P", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLog2Handler("LOG2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLog10Handler("LOG10", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBExpHandler("EXP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBExpM1Handler("EXPM1", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBExp2Handler("EXP2", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBPowHandler("POW", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSqrtHandler("SQRT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBHypotHandler("HYPOT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLCGValueHandler("LCG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBIsFiniteHandler("FIN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBIsInfiniteHandler("INF", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBIsNaNHandler("NAN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBInverseSqrtHandler("ISQRT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBClampHandler("CLAMP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBFractHandler("FRACT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBMixHandler("MIX", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSignHandler("SIGN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBStepHandler("STEP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBSmoothStepHandler("SSTEP", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBRadianToDegreeHandler("RAD2DEG", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDegreeToRadianHandler("DEG2RAD", $elementType, $addressMode);
			}
		} else {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBRandomHandler("RAND", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBRandomMTHandler("MT_RAND", $elementType, $addressMode);
			}
		}
	}

	protected function addFlowControlHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		if(!in_array("QBJumpHandler", array_map("get_class", $this->handlers))) {
			$this->handlers[] = new QBNOPHandler("NOP");
			$this->handlers[] = new QBJumpHandler("JMP");
			$this->handlers[] = new QBReturnHandler("RET");
			$this->handlers[] = new QBExitHandler("EXIT");
			$this->handlers[] = new QBFunctionCallHandler("FCALL", "VAR");
			$this->handlers[] = new QBFunctionCallHandler("FCALL", "MIX");
			$this->handlers[] = new QBBranchIfStaticVariablesInitializedHandler("IF_INIT");
		}
		$branchHandlers = array();
		if(!$unsigned) {
			if($elementTypeNoSign == "I32") {
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new QBBranchOnTrueHandler("IF_T", $elementTypeNoSign, $addressMode);
				}
				foreach($this->scalarAddressModes as $addressMode) {
					$this->handlers[] = new QBBranchOnFalseHandler("IF_F", $elementTypeNoSign, $addressMode);
				}
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBBranchOnEqualHandler("IF_EQ", $elementTypeNoSign, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBBranchOnNotEqualHandler("IF_NE", $elementTypeNoSign, $addressMode);
			}
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new QBBranchOnLessThanHandler("IF_LT", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new QBBranchOnGreaterThanHandler("IF_GT", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new QBBranchOnLessThanOrEqualHandler("IF_LE", $elementType, $addressMode);
		}
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new QBBranchOnGreaterThanOrEqualHandler("IF_GE", $elementType, $addressMode);
		}
	}

	protected function addStringHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$float = preg_match('/^F/', $elementType);
		if($elementType == "U08") {
			$this->handlers[] = new QBPrintStringHandler("PRN_STR", $elementType);
			$this->handlers[] = new QBConcatStringHandler("CAT_STR", $elementType);
			
		}
		if($elementType == "U16" || $elementType == "U32") {
			$this->handlers[] = new QBUTF8DecodeHandler("UTF8_DEC", $elementType);
			$this->handlers[] = new QBUTF8EncodeHandler("UTF8_ENC", $elementType);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBPrintHandler("PRN", $elementType, $addressMode);
		}
		foreach($this->addressModes as $addressMode) {
			$this->handlers[] = new QBConcatVariableHandler("CAT", $elementType, $addressMode);
		}
		if(!$unsigned && $elementTypeNoSign != "I08") {
			foreach($this->scalarAddressModes as $addressMode) {		
				$this->handlers[] = new QBPackHandler("PACK_LE", $elementTypeNoSign, $addressMode, "LITTLE_ENDIAN");
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBPackHandler("PACK_BE", $elementTypeNoSign, $addressMode, "BIG_ENDIAN");
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBUnpackHandler("UNPACK_LE", $elementTypeNoSign, $addressMode, "LITTLE_ENDIAN");
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBUnpackHandler("UNPACK_BE", $elementTypeNoSign, $addressMode, "BIG_ENDIAN");
			} 
		}
	}
	
	protected function addArrayHandlers($elementType) {
		$unsigned = preg_match('/^U/', $elementType);
		$elementTypeNoSign = preg_replace('/^S/', "I", $elementType);
		$this->handlers[] = new QBScalarSortHandler("SORT", $elementType);
		$this->handlers[] = new QBReverseScalarSortHandler("RSORT", $elementType);
		foreach($this->scalarAddressModes as $addressMode) {
			$this->handlers[] = new QBArrayMinHandler("AMIN", $elementType, $addressMode);
			$this->handlers[] = new QBArrayMaxHandler("AMAX", $elementType, $addressMode);
			$this->handlers[] = new QBArrayProductHandler("APROD", $elementType, $addressMode);
			$this->handlers[] = new QBArraySumHandler("ASUM", $elementType, $addressMode);
		}
		if(!$unsigned) {
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBArraySearchHandler("AFIND_IDX", $elementTypeNoSign, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBInArrayHandler("AFIND", $elementTypeNoSign, $addressMode);
			}
			$this->handlers[] = new QBSubarrayPositionHandler("APOS", $elementTypeNoSign);
			$this->handlers[] = new QBSubarrayPositionFromEndHandler("ARPOS", $elementTypeNoSign);
			$this->handlers[] = new QBArrayReverseHandler("AREV", $elementTypeNoSign);
		}
	}
	
	protected function addSamplingHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if($float) {
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBSampleNearestHandler("SAMPLE_NN", $elementType, $addressMode);
			}
			foreach($this->scalarAddressModes as $addressMode) {
				$this->handlers[] = new QBSampleBilinearHandler("SAMPLE_BL", $elementType, $addressMode);
			}
		}
	}
	
	protected function addMatrixHandlers($elementType) {
		$float = preg_match('/^F/', $elementType);
		if($float) {
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM", $elementType, null);
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM", $elementType, "ARR");
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV", $elementType, null);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV", $elementType, "ARR");
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM", $elementType, null);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM", $elementType, "ARR");
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDotProductHandler("DOT", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLengthHandler("LEN", $elementType, $addressMode);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDistanceHandler("DIS", $elementType, $addressMode);
			}
			$this->handlers[] = new QBNormalizeHandler("NORM", $elementType, null);
			$this->handlers[] = new QBNormalizeHandler("NORM", $elementType, "ARR");
			$this->handlers[] = new QBFaceForwardHandler("FORE", $elementType, null);
			$this->handlers[] = new QBFaceForwardHandler("FORE", $elementType, "ARR");
			$this->handlers[] = new QBReflectHandler("REFL", $elementType, null);
			$this->handlers[] = new QBReflectHandler("REFL", $elementType, "ARR");
			$this->handlers[] = new QBRefractHandler("REFR", $elementType, null);
			$this->handlers[] = new QBRefractHandler("REFR", $elementType, "ARR");

			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_4X4", $elementType, null, 4);
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_4X4", $elementType, "ARR", 4);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_4X4", $elementType, null, 4);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_4X4", $elementType, "ARR", 4);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_4X4", $elementType, null, 4);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_4X4", $elementType, "ARR", 4);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDotProductHandler("DOT_4X", $elementType, $addressMode, 4);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLengthHandler("LEN_4X", $elementType, $addressMode, 4);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDistanceHandler("DIS_4X", $elementType, $addressMode, 4);
			}
			$this->handlers[] = new QBNormalizeHandler("NORM_4X", $elementType, null, 4);
			$this->handlers[] = new QBNormalizeHandler("NORM_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBFaceForwardHandler("FORE_4X", $elementType, null, 4);
			$this->handlers[] = new QBFaceForwardHandler("FORE_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBReflectHandler("REFL_4X", $elementType, null, 4);
			$this->handlers[] = new QBReflectHandler("REFL_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBRefractHandler("REFR_4X", $elementType, null, 4);
			$this->handlers[] = new QBRefractHandler("REFR_4X", $elementType, "ARR", 4);
			
			$this->handlers[] = new QBCopyHandler("MOV_4X", $elementType, null, 4);
			$this->handlers[] = new QBCopyHandler("MOV_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBAddHandler("ADD_4X", $elementType, null, 4);
			$this->handlers[] = new QBAddHandler("ADD_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBSubtractHandler("SUB_4X", $elementType, null, 4);
			$this->handlers[] = new QBSubtractHandler("SUB_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBMultiplyHandler("MUL_4X", $elementType, null, 4);
			$this->handlers[] = new QBMultiplyHandler("MUL_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBDivideHandler("DIV_4X", $elementType, null, 4);
			$this->handlers[] = new QBDivideHandler("DIV_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBModuloHandler("MOD_4X", $elementType, null, 4);
			$this->handlers[] = new QBModuloHandler("MOD_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBNegateHandler("NEG_4X", $elementType, null, 4);
			$this->handlers[] = new QBNegateHandler("NEG_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBIncrementHandler("INC_4X", $elementType, null, 4);
			$this->handlers[] = new QBIncrementHandler("INC_4X", $elementType, "ARR", 4);
			$this->handlers[] = new QBDecrementHandler("DEC_4X", $elementType, null, 4);
			$this->handlers[] = new QBDecrementHandler("DEC_4X", $elementType, "ARR", 4);

			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_3X3", $elementType, null, 3);
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_3X3", $elementType, "ARR", 3);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_3X3", $elementType, null, 3);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_3X3", $elementType, "ARR", 3);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_3X3", $elementType, null, 3);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_3X3", $elementType, "ARR", 3);
			if($elementType == "F32") {
				// this is used by Pixel Bender only
				$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_3X3P", $elementType, null, "3+P");
				$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_3X3P", $elementType, null, "3+P");
				$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_3X3P", $elementType, null, "3+P");
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDotProductHandler("DOT_3X", $elementType, $addressMode, 3);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBLengthHandler("LEN_3X", $elementType, $addressMode, 3);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDistanceHandler("DIS_3X", $elementType, $addressMode, 3);
			}
			$this->handlers[] = new QBNormalizeHandler("NORM_3X", $elementType, null, 3);
			$this->handlers[] = new QBNormalizeHandler("NORM_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBCrossProductHandler("CROSS", $elementType, null);
			$this->handlers[] = new QBCrossProductHandler("CROSS", $elementType, "ARR");
			$this->handlers[] = new QBFaceForwardHandler("FORE_3X", $elementType, null, 3);
			$this->handlers[] = new QBFaceForwardHandler("FORE_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBReflectHandler("REFL_3X", $elementType, null, 3);
			$this->handlers[] = new QBReflectHandler("REFL_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBRefractHandler("REFR_3X", $elementType, null, 3);
			$this->handlers[] = new QBRefractHandler("REFR_3X", $elementType, "ARR", 3);
			
			$this->handlers[] = new QBCopyHandler("MOV_3X", $elementType, null, 3);
			$this->handlers[] = new QBCopyHandler("MOV_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBAddHandler("ADD_3X", $elementType, null, 3);
			$this->handlers[] = new QBAddHandler("ADD_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBSubtractHandler("SUB_3X", $elementType, null, 3);
			$this->handlers[] = new QBSubtractHandler("SUB_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBMultiplyHandler("MUL_3X", $elementType, null, 3);
			$this->handlers[] = new QBMultiplyHandler("MUL_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBDivideHandler("DIV_3X", $elementType, null, 3);
			$this->handlers[] = new QBDivideHandler("DIV_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBModuloHandler("MOD_3X", $elementType, null, 3);
			$this->handlers[] = new QBModuloHandler("MOD_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBNegateHandler("NEG_3X", $elementType, null, 3);
			$this->handlers[] = new QBNegateHandler("NEG_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBIncrementHandler("INC_3X", $elementType, null, 3);
			$this->handlers[] = new QBIncrementHandler("INC_3X", $elementType, "ARR", 3);
			$this->handlers[] = new QBDecrementHandler("DEC_3X", $elementType, null, 3);
			$this->handlers[] = new QBDecrementHandler("DEC_3X", $elementType, "ARR", 3);
			
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_2X2", $elementType, null, 2);
			$this->handlers[] = new QBMultiplyMatrixByMatrixHandler("MUL_MM_2X2", $elementType, "ARR", 2);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_2X2", $elementType, null, 2);
			$this->handlers[] = new QBMultiplyMatrixByVectorHandler("MUL_MV_2X2", $elementType, "ARR", 2);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_2X2", $elementType, null, 2);
			$this->handlers[] = new QBMultiplyVectorByMatrixHandler("MUL_VM_2X2", $elementType, "ARR", 2);
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDotProductHandler("DOT_2X", $elementType, $addressMode, 2);
			}
			foreach($this->addressModes as $addressMode) {			
				$this->handlers[] = new QBLengthHandler("LEN_2X", $elementType, $addressMode, 2);
			}
			foreach($this->addressModes as $addressMode) {
				$this->handlers[] = new QBDistanceHandler("DIS_2X", $elementType, $addressMode, 2);
			}
			$this->handlers[] = new QBNormalizeHandler("NORM_2X", $elementType, null, 2);
			$this->handlers[] = new QBNormalizeHandler("NORM_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBFaceForwardHandler("FORE_2X", $elementType, null, 2);
			$this->handlers[] = new QBFaceForwardHandler("FORE_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBReflectHandler("REFL_2X", $elementType, null, 2);
			$this->handlers[] = new QBReflectHandler("REFL_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBRefractHandler("REFR_2X", $elementType, null, 2);
			$this->handlers[] = new QBRefractHandler("REFR_2X", $elementType, "ARR", 2);
			
			$this->handlers[] = new QBCopyHandler("MOV_2X", $elementType, null, 2);
			$this->handlers[] = new QBCopyHandler("MOV_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBAddHandler("ADD_2X", $elementType, null, 2);
			$this->handlers[] = new QBAddHandler("ADD_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBSubtractHandler("SUB_2X", $elementType, null, 2);
			$this->handlers[] = new QBSubtractHandler("SUB_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBMultiplyHandler("MUL_2X", $elementType, null, 2);
			$this->handlers[] = new QBMultiplyHandler("MUL_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBDivideHandler("DIV_2X", $elementType, null, 2);
			$this->handlers[] = new QBDivideHandler("DIV_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBModuloHandler("MOD_2X", $elementType, null, 2);
			$this->handlers[] = new QBModuloHandler("MOD_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBNegateHandler("NEG_2X", $elementType, null, 2);
			$this->handlers[] = new QBNegateHandler("NEG_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBIncrementHandler("INC_2X", $elementType, null, 2);
			$this->handlers[] = new QBIncrementHandler("INC_2X", $elementType, "ARR", 2);
			$this->handlers[] = new QBDecrementHandler("DEC_2X", $elementType, null, 2);
			$this->handlers[] = new QBDecrementHandler("DEC_2X", $elementType, "ARR", 2);
		}
	}
	
	protected function addDebugHandlers() {
		$this->handlers[] = new QBExtensionOpHandler("EXT");
	}
}

?>