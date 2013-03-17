<?php

class QBHandler {
	protected static $typeDecls = array();
	protected static $compiler;
	protected static $scalarAliases = array();

	protected $opCount = 0;
	protected $srcCount = 0;
	protected $dstCount = 0;
	
	protected $operandSize;
	protected $outElementCount;

	protected $baseName;
	protected $operandType;
	protected $addressMode;

	protected $functionUsed = array();
	protected $variableUsed = array();
	protected $flags = 0;
	
	const NEED_LINE_NUMBER						= 0x0001;
	const WILL_JUMP								= 0x0004;
	const IS_ISSET								= 0x0008;
	const IS_UNSET								= 0x0010;
	const WRAP_AROUND_HANDLING					= 0x0020;
	const IS_VECTORIZED							= 0x0040;
	const NEED_MATRIX_DIMENSIONS				= 0x0080;

	const SEARCHING_FOR_OPERANDS				= 0x1000;
	const SEARCHING_FOR_LINE_NUMBER				= 0x2000;
	const SEARCHING_FOR_CALLS_AND_VARIABLES		= 0x4000;
	
	static public function getTypeDeclarations() {
		ksort(self::$typeDecls);
		return self::$typeDecls;
	}
	
	static public function getMacroDefinitions() {
		$lines = array();
		foreach(self::$scalarAliases as $name => $def) {
			$lines[] = "#define $name	$def";
		}
		sort($lines);
		return $lines;
	}
	
	static public function getMacroUndefinitions() {
		$lines = array();
		foreach(self::$scalarAliases as $name => $def) {
			$lines[] = "#undef $name";
		}
		sort($lines);
		return $lines;
	}
	
	static public function setCompiler($compiler) {
		self::$compiler = $compiler;
	}
	
	public function __construct($baseName, $operandType = NULL, $addressMode = NULL, $vectorWidth = null) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->addressMode = $addressMode;
		if($vectorWidth) {
			$this->flags |= self::IS_VECTORIZED;
			if($vectorWidth == "variable") {
				$this->flags |= self::NEED_MATRIX_DIMENSIONS;
			}
			$this->operandSize = $vectorWidth;
		} else {
			$this->operandSize = 1;
		}
		$this->initialize();
		$this->scanCode();
	}
	
	public function getBaseName() {
		return $this->baseName;
	}

	public function getName() {
		if($this->flags & self::SEARCHING_FOR_OPERANDS) {
			// the full name isn't known during the initial scan for op count
			return "NAME";
		}
		
		$name = $this->baseName;
		
		// append operand type to the name
		for($i = 1; $i <= $this->opCount; $i++) {
			$type = $this->getOperandType($i);
			$name .= "_$type";
		}
		
		// append the address mode
		if($this->addressMode) {
			$name .= "_$this->addressMode";
		}
		return $name;
	}

	// return code for the op handler
	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$instr = $this->getInstructionStructure();
		$action = $this->getAction();
		$lines[] = $this->getLabelCode($name);
		$lines[] = $this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler");
		$lines[] = "{";
		if($this->flags & self::NEED_LINE_NUMBER) {
			$lines[] = "#define PHP_LINE_NUMBER	(($instr *) instruction_pointer)->line_number";
		}
		if($this->flags & self::NEED_MATRIX_DIMENSIONS) {
			$lines[] = "#define MATRIX1_ROWS			((($instr *) instruction_pointer)->matrix_dimensions >> 20)";
			$lines[] = "#define MATRIX1_COLS			(((($instr *) instruction_pointer)->matrix_dimensions >> 10) & 0x03FF)";
			$lines[] = "#define MATRIX2_ROWS			MATRIX1_COLS";
			$lines[] = "#define MATRIX2_COLS			((($instr *) instruction_pointer)->matrix_dimensions & 0x03FF)";
		}
		for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] = $this->getOperandDeclaration($i);
		}
		$lines[] = "";
		for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		$lines[] = $action;
		if($this->flags & self::NEED_LINE_NUMBER) {
			$lines[] = "#undef PHP_LINE_NUMBER";
		}
		if($this->flags & self::NEED_MATRIX_DIMENSIONS) {
			$lines[] = "#undef MATRIX1_ROWS";
			$lines[] = "#undef MATRIX1_COLS";
			$lines[] = "#undef MATRIX2_ROWS";
			$lines[] = "#undef MATRIX2_COLS";
		}
		$lines[] = "}";
		$lines[] = "instruction_pointer += sizeof($instr);";
		$lines[] = $this->getJumpCode();
		return $lines;
	}

	// return code for helper functions needed by the handler
	public function getHelperFunctions() {
	}
	
	// return the instruction structure for the op
	public function getInstructionStructure() {		
		if($this->flags & (self::SEARCHING_FOR_OPERANDS | self::SEARCHING_FOR_LINE_NUMBER)) {
			// the structure isn't known during the initial scan for operand count and line number
			return "INSTRUCTION_STRUCTURE";
		}
		$instr = "qb_instruction_$this->opCount";
		if($this->flags & self::NEED_MATRIX_DIMENSIONS) {
			$instr .= "_matrix";
		}
		if($this->flags & self::NEED_LINE_NUMBER) {
			$instr .= "_lineno";
		}
		if(!isset(self::$typeDecls[$instr])) {
			$lines = array();
			$lines[] = "void *next_handler;";
			for($i = 1; $i <= $this->opCount; $i++) {
				$lines[] = "uint32_t operand{$i};";
			}
			if($this->flags & self::NEED_MATRIX_DIMENSIONS) {
				$lines[] = "uint32_t matrix_dimensions;";
			}
			if($this->flags & self::NEED_LINE_NUMBER) {
				$lines[] = "uint32_t line_number;";
			}
			self::$typeDecls[$instr] = array(
					"typedef struct $instr {",
					$lines,
					"} $instr;"
			);
		}
		return $instr;
	}

	// return the C-type of operand $i
	public function getOperandCType($i) {
		static $cTypes = array(
				"I08" => "int8_t",		"I16" => "int16_t",			"I32" => "int32_t",			"I64" => "int64_t",
				"S08" => "int8_t",		"S16" => "int16_t",			"S32" => "int32_t",			"S64" => "int64_t",
				"U08" => "uint8_t",		"U16" => "uint16_t",		"U32" => "uint32_t",		"U64" => "uint64_t",
				"F32" => "float32_t",	"F64" => "float64_t",
		);
		$operandType = $this->getOperandType($i);
		return $cTypes[$operandType];
	}

	// return the type of operand $i
	// by default, all operands have the same type
	public function getOperandType($i) {
		return $this->operandType;
	}
	
	// return the number of elements that consist an operand 
	public function getOperandSize($i) {
		return $this->operandSize;
	}
	
	public function getOperandFlags() {
		$flags = array();
		for($i = 1, $shift = 0; $i <= $this->opCount; $i++, $shift += 2) {
			$mode = $this->getOperandAddressMode($i);
			$operandInfo = "QB_OPERAND_ADDRESS_$mode";
			if($i > $this->srcCount) {
				$operandInfo = "($operandInfo | QB_OPERAND_WRITABLE)";
			}
			$flags[] = $operandInfo;
		}
		return $flags;
	}
	
	public function getOpFlags() {
		$flags = array();
		if($this->flags & self::IS_VECTORIZED) {
			$flags[] = "QB_OP_VECTORIZED";
			if($this->flags & self::NEED_MATRIX_DIMENSIONS) {
				$flags[] = "QB_OP_NEED_MATRIX_DIMENSIONS";
			}
		}
		if($this->addressMode == "VAR") {
			// op can be employed in different address modes
			$flags[] = "QB_OP_MULTI_ADDRESS";
		}
		if($this->flags & self::WILL_JUMP) {
			// the op will redirect execution to another location 
			$flags[] = "QB_OP_JUMP";
		}
		if($this->flags & self::NEED_LINE_NUMBER) {
			// the line number needs to be packed into the instruction (for error reporting)
			$flags[] = "QB_OP_NEED_LINE_NUMBER";
		}
		if($this->flags & self::IS_ISSET) {
			// the behavior of isset and unset are somewhat unique, namely that they can to access an out-of-bound element 
			$flags[] = "QB_OP_ISSET";
		}
		if($this->flags & self::IS_UNSET) {
			$flags[] = "QB_OP_UNSET";
		}
		if($this->flags & self::WRAP_AROUND_HANDLING) {
			$flags[] = "QB_OP_PERFORM_WRAP_AROUND";
		}
		return $flags;
	}
	
	public function getFunctionsUsed() {
		return array_keys($this->functionUsed);
	}

	public function getVariablesUsed() {
		return array_keys($this->variableUsed);
	}

	// return the address mode of operand $i
	// by default, all operands use the same address mode
	public function getOperandAddressMode($i) {
		if($this->addressMode) {
			return $this->addressMode;
		} else {
			if($this->flags & self::IS_VECTORIZED) {
				return "ARR";
			}
		}
	}

	// called by constructor so we can avoid overriding the constructor itself
	protected function initialize() {
	}

	// determine the operand count by scanning through the code for specific variable
	protected function scanCode() {
		$this->flags |= self::SEARCHING_FOR_OPERANDS;
		$lines = $this->getCode();
		$lines = $this->linearizeArray($lines);
		$variableFound = array();
		$variables = array("res", "res_ptr", "res_count", "res_start", "res_start_index", "res_end", "res_count", "res_count_before", "op#", "op#_ptr", "op#_count", "op#_start", "op#_start_index", "op#_end", "op#count");
		$patterns = array();
		foreach($variables as $variable) {
			// \b means word boundary
			$patterns[] = "\b" . str_replace("#", "\d+", $variable) . "\b";
		}
		$pattern = "/" . implode("|", $patterns) . "/";
		foreach($lines as $line) {
			if(preg_match_all($pattern, $line, $matches, PREG_SET_ORDER)) {
				foreach($matches as $match) {
					$variable = $match[0];
					if(!isset($variableFound[$variable])) {
						if(preg_match("/^res(.*)/", $variable, $m)) {
							$this->dstCount = 1;							
							$suffix = $m[1];
							if(!$suffix) {
								self::$scalarAliases["res"] = "(*res_ptr)";
								$variableFound["res_ptr"] = true;
							}							
						} else if(preg_match("/^op(\d+)(.*)/", $variable, $m)) {
							$number = (int) $m[1];
							$suffix = $m[2];
							if($number > $this->srcCount) {
								$this->srcCount = $number;
							}
							if(!$suffix) {
								self::$scalarAliases["op{$number}"] = "(*op{$number}_ptr)";
								$variableFound["op{$number}_ptr"] = true;
							}
						}
						$variableFound[$variable] = true;
					}
				}
			}
		}
		
		$this->opCount = $this->srcCount + $this->dstCount;
		$this->flags &= ~self::SEARCHING_FOR_OPERANDS;
		
		// scan again now that the operand count is set to see if the handler needs the line number (for error output)
		$this->flags |= self::SEARCHING_FOR_LINE_NUMBER;
		$lines = $this->getCode();
		$lines = $this->linearizeArray($lines);
		foreach($lines as $line) {
			if(preg_match('/\bPHP_LINE_NUMBER\b/', $line)) {
				$this->flags |= self::NEED_LINE_NUMBER;
			}
		}
		$this->flags &= ~self::SEARCHING_FOR_LINE_NUMBER;
		
		// scan the code for function calls and variables now that the code is complete
		$this->flags |= self::SEARCHING_FOR_CALLS_AND_VARIABLES;
		$lines = $this->getCode();
		$lines = $this->linearizeArray($lines);
		$functionFound = array();
		$variableFound = array();
		foreach($lines as $line) {		
			if(!preg_match('/^\s*#/', $line) && preg_match('/(\w+)\s*\(/', $line, $m)) {
				$token = $m[1];
				switch($token) {
					case 'if':
					case 'for':
					case 'while':
					case 'sizeof':
					case 'QB_G':
					case 'EG':
					case 'SWAP_LE_I16':
					case 'SWAP_LE_I32':
					case 'SWAP_LE_I64':
					case 'SWAP_BE_I16':
					case 'SWAP_BE_I32':
					case 'SWAP_BE_I64':
						break;
					default:
						$functionFound[$token] = true;
				}					
			}
			if(preg_match_all($pattern, $line, $matches, PREG_SET_ORDER)) {
				foreach($matches as $match) {
					$variable = $match[0];
					$variableFound[$variable] = true;
				}
			}
		}
		$this->functionUsed = $functionFound;
		$this->variableUsed = $variableFound;
		$this->flags &= ~self::SEARCHING_FOR_CALLS_AND_VARIABLES;
	}
	
	// linearize an array
	protected function linearizeArray($array) {
		$result = array();
		foreach($array as $element) {
			if($element !== NULL) {
				if(is_array($element)) {
					$sub_array = $this->linearizeArray($element);					
					array_splice($result, count($result), 0, $sub_array);
				} else {
					$result[] = $element;
				}
			}
		}
		return $result;
	}
	
	// return code for the handle label	
	protected function getLabelCode($name) {
		if(self::$compiler == "GCC") {
			return "label_$name:";
		} else if(self::$compiler == "MSVC") {
			return "case QB_$name:";
		}
	}
	
	// return code for setting the next op handler
	protected function getSetHandlerCode($value) {
		return "op_handler = $value;";
	}

	// return code for jumping to the next op handler
	protected function getJumpCode() {
		if(self::$compiler == "GCC") {
			return "goto *op_handler;";
		} else if(self::$compiler == "MSVC") {
			return "break;";
		}
	}
	
	protected function getTimeoutCode() {
		if(self::$compiler == "MSVC") {
			$lines = array();
			$lines[] = "#ifdef ZEND_WIN32";
			$lines[] = "if(*windows_timed_out_pointer) {";
			$lines[] =		"zend_timeout(1);";
			$lines[] = 		"goto label_exit;";
			$lines[] = "}";
			$lines[] = "#endif";
			return $lines;
		}
	}
	
	// return the variable declaration needed for retrieving operand $i
	// this method is needed mainly because of the limitations of Visual C  
	protected function getOperandDeclaration($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		$lines = array();
		if($addressMode == "VAR") {
			// selector is fixed to QB_SEGMENT_SCALAR
			if($i <= $this->srcCount) {
				$lines[] = $this->declareVariables($cType, array("op{$i}_ptr" => "*__restrict op{$i}_ptr"));
			} else {
				$lines[] = $this->declareVariables($cType, array("res_ptr" => "*__restrict res_ptr"));
			}
		} else if($addressMode == "ELC") {
			// segment selector is encoded, along with a fixed offset
			if($i <= $this->srcCount) {
				$lines[] = $this->declareVariables($cType, array("op{$i}_ptr" => "*__restrict op{$i}_ptr"));
			} else {
				$lines[] = $this->declareVariables($cType, array("res_ptr" => "*__restrict res_ptr"));
			}
		} else if($addressMode == "ELV") {
			// segment selector plus the index of a variable in segment 0 (i.e. QB_SEGMENT_SCALAR)
			// which will serve as the index for the array element
			if($i <= $this->srcCount) {
				$lines[] = $this->declareVariables($cType, array("op{$i}_ptr" => "*__restrict op{$i}_ptr"));
			} else {
				$lines[] = $this->declareVariables($cType, array("res_ptr" => "*__restrict res_ptr"));
			}
		} else if($addressMode == "ARR") {
			// a array address, containing two indices besides the segment selector
			// both points to variable in segment 0--one is the offset and the other the size
			if($i <= $this->srcCount) {
				$lines[] = $this->declareVariables("uint32_t", array("op{$i}_start_index" => "op{$i}_start_index", "op{$i}_count" => "op{$i}_count"));
				$lines[] = $this->declareVariables($cType, array("op{$i}_start" => "*op{$i}_start", "op{$i}_end" => "*op{$i}_end", "op{$i}_ptr" => "*__restrict op{$i}_ptr"));
			} else {
				$lines[] = $this->declareVariables("uint32_t", array("res_start_index" => "res_start_index", "res_count" => "res_count", "res_count_before" => "res_count_before"));
				$lines[] = $this->declareVariables($cType, array("res_start" => "*res_start", "res_end" => "*res_end", "res_ptr" => "*__restrict res_ptr"));
			}
		}
		return $lines;
	}
	
	protected function declareVariables($cType, $decls) {
		$line = NULL;
		if($this->variableUsed) {
			foreach($decls as $var => $decl) {
				if(isset($this->variableUsed[$var])) {
					if(!$line) {
						$line = "$cType $decl";
					} else {
						$line .= ", $decl";
					}
				}
			}
			if($line) {
				$line .= ";";
			}
		}
		return $line;
	}	
	
	// return code for retrieving operand $i
	// variables employed here should be declared in getOperandDeclaration() 
	protected function getOperandRetrievalCode($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		if($addressMode == "VAR") {
			$lines[] = "index = (($instr *) instruction_pointer)->operand{$i};";
			if($i <= $this->srcCount) {
				$lines[] = "op{$i}_ptr = (($cType *) segment0) + index;";
			} else {
				$j = $i - $this->srcCount;
				$lines[] = "res_ptr = (($cType *) segment0) + index;";
			}
		} else if($addressMode == "ELC" || $addressMode == "ELV") {
			if($addressMode == "ELC") {
				$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
				$lines[] = "index = (($instr *) instruction_pointer)->operand{$i} >> 8;";
			} else {
				$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
				$lines[] = "index_selector = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x00FF;";
				$lines[] = "index_index = (($instr *) instruction_pointer)->operand{$i} >> 16;";
				$lines[] = "index = ((uint32_t *) segments[index_selector])[index_index];";
			}
			if($i <= $this->srcCount) {				
				if(!($this->flags & self::IS_ISSET)) {
					// abort on out-of-bound
					$lines[] = "if(UNEXPECTED(index >= segment_element_counts[selector])) {";
					$lines[] = 		"qb_abort_range_error(cxt, &cxt->storage->segments[selector], index, 1, PHP_LINE_NUMBER);";
					$lines[] = "}";
				} else {
					// set pointer to null
					$lines[] = "if(index >= segment_element_counts[selector]) {";
					$lines[] = 		"op{$i}_ptr = NULL;";
					$lines[] = "} else {";
				}
				$lines[] = "op{$i}_ptr = (($cType *) segments[selector]) + index;";				
				if($this->flags & self::IS_ISSET) {
					$lines[] = "}"; // end else
				}
			} else {
				if(!($this->flags & self::IS_UNSET)) {
					// expand the segment or abort
					$lines[] = "if(segment_expandable[selector]) {";
					$lines[] = 		"if(index >= segment_element_counts[selector]) {";
					$lines[] = 			"qb_enlarge_segment(cxt, &cxt->storage->segments[selector], index + 1);";
					$lines[] = 		"}";
					$lines[] = 	"} else {";
					$lines[] = 		"if(UNEXPECTED(index >= segment_element_counts[selector])) {";
					$lines[] =			"qb_abort_range_error(cxt, &cxt->storage->segments[selector], index, 1, PHP_LINE_NUMBER);";
					$lines[] = 		"}";
					$lines[] = "}";
					$lines[] = "res_ptr = (($cType *) segments[selector]) + index;";
				} else {
					// shrink the segment at index
					$lines[] = "if(index < segment_element_counts[selector]) {";
					$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], index, 1);";
					$lines[] = "}";
				}
			}
		} else if($addressMode == "ARR") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x03FF;";
			$lines[] = "size_index = (($instr *) instruction_pointer)->operand{$i} >> 20;";
			if($i <= $this->srcCount) {
				$lines[] = "op{$i}_start_index = ((uint32_t *) segment0)[index_index];";
				$lines[] = "op{$i}_count = ((uint32_t *) segment0)[size_index];";
				if(!($this->flags & self::IS_ISSET)) {
					$lines[] = "if(UNEXPECTED(op{$i}_start_index + op{$i}_count > segment_element_counts[selector] || op{$i}_start_index + op{$i}_count < op{$i}_start_index)) {";
					$lines[] = 		"qb_abort_range_error(cxt, &cxt->storage->segments[selector], op{$i}_start_index, op{$i}_count, PHP_LINE_NUMBER);";
					$lines[] = "}";
				} else {
					$lines[] = "if(op{$i}_start_index + op{$i}_count > segment_element_counts[selector]) {";
					$lines[] = 		"op{$i}_ptr = NULL;";
					$lines[] = "} else {";
				}
				if(isset($this->variableUsed["op{$i}_start"])) {
					$lines[] = "op{$i}_ptr = op{$i}_start = (($cType *) segments[selector]) + op{$i}_start_index;";
				} else {
					$lines[] = "op{$i}_ptr = (($cType *) segments[selector]) + op{$i}_start_index;";
				}
				if(isset($this->variableUsed["op{$i}_end"])) {
					$lines[] = "op{$i}_end = op{$i}_ptr + op{$i}_count;";
				}
				if($this->flags & self::IS_ISSET) {
					$lines[] = "}";	// end else
				}
			} else {
				$lines[] = "res_start_index = ((uint32_t *) segment0)[index_index];";
				$lines[] = "res_count = res_count_before = ((uint32_t *) segment0)[size_index];";				
				if(!($this->flags & self::IS_UNSET)) {
					$result_size_possiblities = $this->getResultSizePossibilities();
					if(!is_array($result_size_possiblities)) {
						$result_size_possiblities = ($result_size_possiblities !== null) ? array($result_size_possiblities) : array();
					}
					$lines[] = $this->getResultSizeCalculation();
					// set res_count to the largest of the possible values
					foreach($result_size_possiblities as $expr) {
						if(preg_match('/res_count \+/', $expr)) {
							// if the expression involves adding to the res_count, then it's obviously going to be bigger than it
							$lines[] = "res_count = $expr;";
						} else {
							$lines[] = "if($expr > res_count) {";
							$lines[] = 		"res_count = $expr;";
							$lines[] = "}";
						}
					}
					$lines[] = "if(segment_expandable[selector]) {";
					$lines[] = 		"if(res_start_index + res_count > segment_element_counts[selector]) {";
					$lines[] = 			"qb_enlarge_segment(cxt, &cxt->storage->segments[selector], res_start_index + res_count);";
					$lines[] = 		"} else if(UNEXPECTED(res_start_index + res_count < res_start_index)) {";
					$lines[] =			"qb_abort_range_error(cxt, &cxt->storage->segments[selector], res_start_index, res_count, PHP_LINE_NUMBER);";
					$lines[] =		"}";
					$lines[] = "} else {";
					$lines[] = 		"if(UNEXPECTED(res_count > res_count_before || res_start_index + res_count > segment_element_counts[selector] || res_start_index + res_count < res_start_index)) {";
					$lines[] =			"qb_abort_range_error(cxt, &cxt->storage->segments[selector], res_start_index, res_count, PHP_LINE_NUMBER);";
					$lines[] = 		"}";
					$lines[] = "}";
					if(isset($this->variableUsed["res_start"])) {
						$lines[] = "res_ptr = res_start = (($cType *) segments[selector]) + res_start_index;";
					} else {
						$lines[] = "res_ptr = (($cType *) segments[selector]) + res_start_index;";
					}
					if(isset($this->variableUsed["res_end"])) {
						$lines[] = "res_end = res_ptr + res_count;";
					}
				} else {
					$lines[] = "if(res_start_index + res_count <= segment_element_counts[selector] && res_start_index + res_count >= res_start_index) {";
					$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], res_start_index, res_count);";
					$lines[] = "}";
				}
			}
		} else {
			$className = get_class($this);
			die("Invalid address mode for operand $i in $className\n");
		}
		$lines[] = "";
		return $lines;
	}
	
	// return list of expressions that could be the new result size (if it's bigger)
	// the default implementation returns the sizes of all array operands
	public function getResultSizePossibilities() {
		if($this->dstCount) {
			$dstMode = $this->getOperandAddressMode($this->srcCount + 1);
			if($dstMode == "ARR") {
				$list = array();
				for($i = 1; $i <= $this->srcCount; $i++) {
					if($this->getOperandAddressMode($i) == "ARR") {
						$list[] = "op{$i}_count";
					}
				}
				if($list) {
					return $list;
				}
			}
		}
	}

	// return code for calculating the values of variables return by getResultSizePossibilities()
	public function getResultSizeCalculation() {
	}
	
	// multiple a scalar operation multiple times
	protected function getUnrolledCode($expression, $count) {
		$arrayOperands = array();
		for($i = 1; $i <= $this->srcCount; $i++) {
			if($this->getOperandAddressMode($i) == "ARR") {
				$arrayOperands[] = $i;
			}
		}
		$nums = implode("|", $arrayOperands);
		$lines = array();
		for($i = 0; $i < $count; $i++) {
			$patterns = array('/\bres\b/', '/\bop(' . $nums . ')\b/');
			$replacements = array("res_ptr[{$i}]", "op\\1_ptr[{$i}]");
			$lines[] = preg_replace($patterns, $replacements, $expression);
		}
		return $lines;
	}

	// return code for a loop that performs the same operation on all element of an array 
	protected function getIterationCode($expression) {
		$lines = array();		
		// make sure none of the input operands are empty		
		$conditions = array();
		if($this->srcCount > 0) {
			for($i = 1; $i <= $this->srcCount; $i++) {
				if($this->getOperandAddressMode($i) == "ARR") {
					$operandSize = $this->getOperandSize($i);
					if($operandSize) {	
						$conditions[] = "op{$i}_count != 0";
					}
				}
			}
			$conditions = implode(" && ", $conditions);
			$lines[] = "if($conditions) {";
		}
		$lines[] = "while(res_ptr < res_end) {";
		$lines[] = 		$expression;
		for($i = 1; $i <= $this->srcCount; $i++) {
			$operandSize = $this->getOperandSize($i);
			if($this->getOperandAddressMode($i) == "ARR") {
				if($operandSize) {
					$lines[] = 	"op{$i}_ptr += $operandSize;";
					$lines[] = 	"if(op{$i}_ptr >= op{$i}_end) {";
					$lines[] = 		"op{$i}_ptr = op{$i}_start;";
					$lines[] = 	"}";
				}
			}
		}
		$operandSize = $this->getOperandSize($this->srcCount + 1);
		if($operandSize) {
			$lines[] = 	"res_ptr += $operandSize;";
		}
		$lines[] = "}";
		if($this->srcCount > 0) {
			$lines[] = "}"; // end if
		}
		
		// if this code is used, then the op is capable of handling situations where wrap-around is necessary
		if($this->srcCount > 1) {
			$this->flags |= self::WRAP_AROUND_HANDLING;
		}
		return $lines;
	}
	
	// return an expression for handling array operands
	// the default implementation creates basic a loop 
	protected function getArrayExpression() {
		$expr = $this->getScalarExpression();
		if($expr) {
			if($this->flags & self::IS_VECTORIZED) {
				// turn a basic op into a SIMD-like op
				$expr = $this->getUnrolledCode($expr, $this->operandSize);
			}
			return $this->getIterationCode($expr);
		}
	}

	// return an expression for handling scalar operands
	protected function getScalarExpression() {
		return null;
	}

	// return codes that perform what the op is supposed to do
	// the default implementation calls getArrayExpression() or $this->getScalarExpression()
	public function getAction() {
		if(!isset($this->addressMode) && !($this->operandSize > 1)) {
			$opName = $this->baseName;
			$className = get_class($this);
			throw new Exception("$opName ($className) is neither a multi-address or a vector operation; the default implementation of getAction() cannot be used");
		}
		if($this->addressMode == "ARR") {
			return $this->getArrayExpression();
		} else {
			$expr = $this->getScalarExpression();
			if($this->flags & self::IS_VECTORIZED) {
				$expr = $this->getUnrolledCode($expr, $this->operandSize);
			}
			return $expr;
		}
	}
}

?>