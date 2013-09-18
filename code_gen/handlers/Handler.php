<?php

class Handler {
	protected static $typeDecls = array();
	protected static $compiler;

	protected $baseName;
	protected $operandType;
	protected $operandSize;
	protected $addressMode;
	protected $multipleData;
	
	static public function getTypeDeclarations() {
		ksort(self::$typeDecls);
		return self::$typeDecls;
	}
	
	static public function setCompiler($compiler) {
		self::$compiler = $compiler;
	}
	
	public function getBaseName() {
		return $this->baseName;
	}

	public function getName() {
		$name = $this->baseName;
		
		// append the vector size
		if($this->operandSize > 1) {
			$name .= "_{$this->operandSize}X";
		}
		
		// append operand type to the name
		$opCount = $this->getOperandCount();
		for($i = 1; $i <= $opCount; $i++) {
			$type = $this->getOperandType($i);
			$name .= "_$type";
		}

		if($this->multipleData) {
			$name .= "_MIO";
		} else {
			// append the address mode
			if($this->addressMode) {
				$name .= "_$this->addressMode";
			}
		}
		return $name;
	}

	// return code for the op handler
	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$instr = $this->getInstructionStructure();
		$action = 
		$opCount = $this->getOperandCount();
		$targetCount = $this->getJumpTargetCount();
		$lines[] = $this->getLabelCode($name);
		if($targetCount == 2) {
			// assume the first branch is taken
			$lines[] = "{";
			$lines[] = 		"int32_t condition;";
			$lines[] = 		$this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler1");
			$lines[] = 		$this->getAction();
			$lines[] = 		"if(condition) {";
			$lines[] = 			"instruction_pointer = (($instr *) instruction_pointer)->instruction_pointer1;";
			$lines[] = 		"} else {";
			$lines[] = 			$this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler2");
			$lines[] = 			"instruction_pointer = (($instr *) instruction_pointer)->instruction_pointer2;";
			$lines[] = 		"}";
			$lines[] = "}";
			$lines[] = $this->getJumpCode();
		} else if($targetCount == 0 || $targetCount == 1) {
			// regular, non-jump instruction goes to the next instruction
			// a unconditional jump instruction goes to the jump target
			$lines[] = "{";
			$lines[] = 		$this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler");
			$lines[] = 		$this->getAction();
			$lines[] = "}";
			if($this->isVariableLength()) {
				$lines[] = "instruction_pointer += (($instr *) instruction_pointer)->length;";
			} else {
				$lines[] = "instruction_pointer += sizeof($instr);";
			}
			$lines[] = $this->getJumpCode();
		} else {
			// end of execution
			$lines[] = "{";
			$lines[] = 		$this->getAction();
			$lines[] = "}";
		}
		return $lines;
	}

	// return code for helper functions needed by the handler
	public function getHelperFunctions() {
	}
	
	// return the instruction structure for the op
	public function getInstructionStructure() {		
		$opCount = $this->getOperandCount();
		$targetCount = $this->getJumpTargetCount();
		$instr = "qb_instruction";
		
		if($targetCount == 2) {
			$instr .= "_branch";
		} else if($targetCount == 1) {
			$instr .= "_jump";
		}
		
		for($i = 1; $i <= $opCount; $i++) {
			$addressMode = $this->getOperandAddressMode($i);
			$instr .= "_{$addressMode}";
		}
		
		if($this->needsLineNumber()) {
			$instr .= "_lineno";
		}
		return $instr;
	}
	
	// return the instruction structure definition
	public function getInstructionStructureDefinition() {
		$instr = $this->getInstructionStructure();
		$targetCount = $this->getJumpTargetCount();
		$opCount = $this->getOperandCount();
		$lines = array();
		$lines[] = "typedef struct $instr {";
		if($targetCount == 2) {
			$lines[] = "void *next_handler1;";
			$lines[] = "int8_t *instruction_pointer1;";
			$lines[] = "void *next_handler2;";
			$lines[] = "int8_t *instruction_pointer2;";
		} else if($targetCount == 1) {
			$lines[] = "void *next_handler;";
			$lines[] = "int8_t *instruction_pointer;";
		} else {
			$lines[] = "void *next_handler;";
		}
		
		for($i = 1; $i <= $opCount; $i++) {
			$addressMode = $this->getOperandAddressMode($i);
			$lines[] = "qb_pointer_{$addressMode} operand{$i};";
		}
		
		if($this->needsLineNumber()) {
			$lines[] = "uint32_t line_number;";
		}
		$lines[] = "} $instr;";
		return $lines;
	}
	
	public function getInstructionFormat() {
		$srcCount = $this->getInputOperandCount();
		$dstCount = $this->getOutputOperandCount();
		$format = "";
		
		for($i = 1; $i <= $srcCount; $i++) {
			$addressMode = $this->getOperandAddressMode($i);
			switch($addressMode) {
				case 'SCA': $format .= 's'; break;
				case 'ELE': $format .= 'e'; break;
				case 'ARR': $format .= 'a'; break;
			}
		}

		for($i = 1; $i <= $dstCount; $i++) {
			$addressMode = $this->getOperandAddressMode($i);
			switch($addressMode) {
				case 'SCA': $format .= 'S'; break;
				case 'ELE': $format .= 'E'; break;
				case 'ARR': $format .= 'A'; break;
			}
		}
		return $format;
	}
	
	// return the number of input operands
	public function getInputOperandCount() {
		return 0;
	}
	
	// return the number of output operands 
	public function getOutputOperandCount() {
		return 0;
	}
	
	// return the number of jump targets
	public function getJumpTargetCount() {
		return 0;
	}

	// return the total number of operands
	public function getOperandCount() {
		return $this->getInputOperandCount() + $this->getOutputOperandCount(); 
	}
	
	// return the type of operand $i (starting from 1)
	// by default, all operands have the same type
	public function getOperandType($i) {
		return $this->operandType;
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
		if(!$operandType) {
			$className = get_class($this);
			throw new Exception("$i is not a valid index for $className");
		}
		return $cTypes[$operandType];
	}
	
	// return the number of elements that consist an operand 
	public function getOperandSize($i) {
		return $this->operandSize;
	}
	
	// return the address mode of operand $i
	// by default, all operands use the same address mode
	public function getOperandAddressMode($i) {
		return $this->addressMode;
	}
	
	public function isMultipleData() {
		return $this->multipleData;
	}	
	
	public function isVariableLength() {
		return false;
	}
	
	public function needsUnrolling() {
		return $this->isVectorized();
	}
	
	public function performsWrapAround() {
		if($this->addressMode == "ARR" && !$this->isOverridden('getActionOnMultipleData')) {
			return true;
		}
		return false;
	}

	public function getFunctionName($prefix) {
		$className = get_class($this);
		$opName = preg_replace("/([a-z])([A-Z])/", "$1_$2", $className);
		$opName = strtolower($opName);
		$name = "qb_{$prefix}_{$opName}";
		if($this->operandSize != 1 && is_int($this->operandSize)) {
			$name .= "_{$this->operandSize}x";
		}
		if($this->isMultipleData()) {
			$name .= "_multiple_times";
		}
		if($this->operandType) {
			$name .= "_{$this->operandType}";
		}
		return $name;
	}
	
	public function getHandlerFunctionName() {
		return $this->getFunctionName("do");
	}
	
	public function getHandlerFunctionType() {
		if(!$this->isOverridden('getAction') && !$this->isOverridden('getCode')) {
			if($this->isMultipleData()) {
				return 'extern';
			} else {
				$action = $this->getActionOnUnitData();
				$count = count($action, true);
				if($count > 16) {
					return 'extern';
				} else {
					$lines = array_linearize($action);
					$hasLoop = false;
					foreach($lines as $line) {
						if(preg_match('/\b(for|while)\b/', $line)) {
							$hasLoop = true;
						}
					}
					if($hasLoop) {
						return 'extern';
					} else {
						return 'inline';
					}
				}
			}
		}
		return null;
	}

	public function getHandlerFunctionDefinition() {
		$functionType = $this->getHandlerFunctionType();
		if($functionType) {
			$function = $this->getHandlerFunctionName();
			$parameterList = $this->getHandlerFunctionParameterList();
			$expressions = $this->getActionExpressions();
			switch($functionType) {
				case 'inline': $typeDecl = "static zend_always_inline void"; break;
				case 'extern': $typeDecl = "void ZEND_FASTCALL"; break;
			}
			
			// replace op# with (*op#_ptr) for array operands and res with (*res_ptr)
			$expressions = is_array($expressions) ? array_linearize($expressions) : array($expressions);
			$srcCount = $this->getInputOperandCount();
			$arrayOperands = array();
			for($i = 1; $i <= $srcCount; $i++) {
				if($this->getOperandAddressMode($i) == "ARR") {
					$arrayOperands[] = $i;
				}
			}
			if($arrayOperands) {
				$inputOperandRegExp = '/\bop(' . implode('|', $arrayOperands) . ')\b/';
			}
			foreach($expressions as &$expression) {
				if($arrayOperands) {
					$expression = preg_replace($inputOperandRegExp, '(*op\1_ptr)', $expression);
				}
				$expression = preg_replace('/\bres\b/', '(*res_ptr)', $expression);
			}
			
			$lines = array();			
			$lines[] = "$typeDecl $function($parameterList) {";
			$lines[] = $expressions;
			$lines[] = "}";
			return $lines;
		}
		return null;
	}
	
	public function getHandlerFunctionParameterList($forDeclaration) {
		$instr = $this->getInstrunctionStructure();
		$srcCount = $this->getInputOperandCount();
		$opCount = $this->getOperandCount();
		$params = array();
		for($i = 1; $i <= $srcCount; $i++) {
			$cType = $this->getOperandCType($i);
			$addressMode = $this->getOperandAddressMode($i);
			$operand = "((($instr *) instruction_pointer)->operand$i)";
			switch($addressMode) {
				case 'SCA':
					if($forDeclaration) {
						$params[] = "$cType op{$i}";
					} else {
						$params[] = "(($cType *) $operand.data_pointer))[0]";
					}
					break;
				case 'ELE':
					if($forDeclaration) {
						$params[] = "$cType op{$i}";
					} else {
						$params[] = "(($cType *) $operand.data_pointer))[$operand.index_pointer[0]]";
					}
					break;
				case 'ARR':
					if($forDeclaration) {
						$params[] = "$cType *op{$i}_ptr";
						$params[] = "uint32_t op{$i}_count";
					} else {
						$params[] = "(($cType *) $operand.data_pointer) + $operand.index_pointer[0]";
						$params[] = "$operand.count_pointer[0]";
					}
					break;
			}
		}
		if($dstCount) {
			$cType = $this->getOperandCType($i);
			$addressMode = $this->getOperandAddressMode($i);
			$operand = "((($instr *) instruction_pointer)->operand$i)";
			switch($addressMode) {
				case 'SCA':
					if($forDeclaration) {
						$params[] = "$cType *res_ptr";
					} else {
						$params[] = "&(($cType *) $operand.data_pointer))[0]";
					}
					break;
				case 'ELE':
					if($forDeclaration) {
						$params[] = "$cType *res_ptr";
					} else {
						$params[] = "&(($cType *) $operand.data_pointer))[$operand.index_pointer[0]]";
					}
					break;
				case 'ARR':
					if($forDeclaration) {
						$params[] = "$cType *res_ptr";
						$params[] = "uint32_t res_count";
					} else {
						$params[] = "(($cType *) $operand.data_pointer) + $operand.index_pointer[0]";
						$params[] = "$operand.count_pointer[0]";
					}
					break;
			}
		}	
		return implode(", ", $params);
	}

	public function getDispatcherFunctionName() {
		return $this->getFunctionName("dispatch");
	}
	
	public function getDispatcherFunctionParameterList($forDeclaration) {
		$instr = $this->getInstrunctionStructure();
		$params = array();
		if($forDeclaration) {
			$params[] = "qb_interpreter_context *__restrict cxt";
			$params[] = "$instr *__restrict instr";
		} else {
			$params[] = "cxt";
			$params[] = "($instr *) instruction_pointer";
		}
		return implode(", ", $params);
	}
	
	public function getDispatcherFunctionDefinitions() {
		$instr = $this->getInstrunctionStructure();
		$dispatcherFunction = $this->getDispatcherFunctionName();
		$dispatcherParameterList = $this->getHandlerFunctionParameterList(true);
		$dispatcherTypeDecl = "void";
		$handlerFunction = $this->getHandlerFunctionName();
		$handlerParameterList = $this->getHandlerFunctionParameterList(false);
		$lines[] = array();
		$lines[] = "$dispatcherTypeDecl $dispatcherFunction($dispatcherParameterList) {";
		$lines[] =		"if(cxt->thread_count_for_next_op) {";
		$lines[] =			"uint32_t j;";
		$lines[] =			"$instr new_instr_list[MAX_THREAD_COUNT];";
		$lines[] =			"for(j = 0; j < cxt->thread_count_for_next_op; j++) {";
		$lines[] =				"$instr *new_instr = &instruction_buffers[j];";
		$lines[] =				"qb_pointer_adjustment *adj;";
		// create temporary instruction structures
		for($i = 1, $k = 0; $i < $opCount; $i++) {
			$addressMode = $this->getOperandAddressMode($i);
			$cType = $this->getOperandCType($i);
			$operandSize = $this->getOperandSize($i);
			if($addressMode == "ARR" && $operandSize > 0) {
				$lines[] =		"adj = cxt->adjustments_for_next_op[j][$k];";
				$lines[] =		"new_instr->operand{$i}.data_pointer = (($cType *) instr->operand{$i}.data_pointer) + instr->operand{$i}.index_pointer[0];";
				$lines[] =		"new_instr->operand{$i}.index_pointer = &adj->index;";
				$lines[] =		"new_instr->operand{$i}.size_pointer = &adj->count;";
				$k++;
			} else {
				$lines[] =		"new_instr->operand{$i} = (($cType *) instr->operand{$i};";
			}
		}
		$lines[] =			"}";
		$lines[] =			"qb_dispatch_instruction_to_threads(cxt, $dispatcherFunction, new_instr_list);";
		$lines[] = 		"} else {";
		$lines[] = 			"$handlerFunction($handlerParameterList);";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
	
	// return codes that perform what the op is supposed to do
	public function getAction() {
		$functionType = $this->getFunctionType();
		if($functionType) {	
			if(!$this->isMultipleData()) {
				// call the handler directly
				$function = $this->getHandlerFunctionName();
				$parameterList = $this->getHandlerFunctionParameterList(false);
			} else {
				// use the dispatcher function
				$function = $this->getDispatcherFunctionName();
				$parameterList = $this->getDispatcherFunctionParameterList(false);
			}
			return "$function($parameterList);";
		} else {
			return $this->getActionExpressions();
		}
	}	
	
	// return an expression for handling a single unit of data (typically a scalar)
	protected function getActionOnUnitData() {
		return null;
	}

	// return an expression for handling multiple units of data
	protected function getActionOnMultipleData() {
		return null;
	}
	
	protected function getActionExpressions() {
		if($this->isMultipleData()) {
			$action = $this->getActionOnMultipleData();
			if(!$action) {
				// change the address mode temporarily
				$originalAddressMode = $this->addressMode;
				$this->addressMode = ($this->operandSize > 1) ? null : "SCA";
				$scalarExpression = $this->getAction();
				$this->addressMode = $originalAddressMode;
				$action = $this->getIterationCode($scalarExpression);
			}
		} else {
			$action = $this->getActionOnUnitData();
			if($this->needsUnrolling()) {
				$action = $this->getUnrolledCode($action, $this->operandSize);
			}
		}
		return $action;
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
	
	// return code for timeout check (Windows only)
	protected function getTimeoutCode() {
		if(self::$compiler == "MSVC") {
			$lines = array();
			$lines[] = "#ifdef ZEND_WIN32";
			$lines[] = "if(windows_timeout_check_counter == 4096) {";
			$lines[] = 		"if(*windows_timed_out_pointer) {";
			$lines[] =			"zend_timeout(1);";
			$lines[] = 			"goto label_exit;";
			$lines[] =		"}";
			$lines[] = "} else {";
			$lines[] =		"windows_timeout_check_counter++;";
			$lines[] = "}";
			$lines[] = "#endif";
			return $lines;
		}
	}	
	
	// multiple a scalar operation multiple times
	protected function getUnrolledCode($expression, $count) {
		$srcCount = $this->getInputOperandCount();
		$arrayOperands = array();
		for($i = 1; $i <= $srcCount; $i++) {
			if($this->getOperandAddressMode($i) == "ARR") {
				$arrayOperands[] = $i;
			}
		}
		$nums = implode("|", $arrayOperands);
		$lines = array();
		for($i = 0; $i < $count; $i++) {
			$patterns = array('/\bres\b/', '/\bop(' . $nums . ')\b/');
			$replacements = array("res_ptr[{$i}]", "op\\1_ptr[{$i}]");
			if(is_array($expression)) {
				foreach($expression as $subexpression) {
					$lines[] = preg_replace($patterns, $replacements, $subexpression);
				}
			} else {
				$lines[] = preg_replace($patterns, $replacements, $expression);
			}
		}
		return $lines;
	}
	
	// return code for a loop that performs the same operation on all element of an array 
	protected function getIterationCode($expression) {
		$srcCount = $this->getInputOperandCount();
		$lines = array();		
		
		// make sure none of the input operands are empty		
		$condition = false;
		$operandCounts = array();
		for($i = 1; $i <= $srcCount; $i++) {
			$operandSize = $this->getOperandSize($i);
			if($this->getOperandAddressMode($i) == "ARR" && $operandSize !== 0) {
				$operandCounts[] = "op{$i}_count";
			}
		}
		$operandCounts[] = "res_count";
		// use bitwise AND here, just in case the compiler doesn't optimize correctly
		$condition = implode(" && ", $operandCounts);
		$lines[] = "if($condition) {";
		for($i = 1; $i <= $srcCount; $i++) {
			$cType = $this->getOperandCType($i);
			$operandSize = $this->getOperandSize($i);
			if($this->getOperandAddressMode($i) == "ARR" && $operandSize !== 0) {
				$lines[] =	"$cType *op{$i}_start = op{$i}_ptr, *op{$i}_end = op{$i}_ptr + op{$i}_count;";
			}
		}
		$cType = $this->getOperandCType($srcCount + 1);
		$lines[] =		"$cType *res_end = res_ptr + res_count;";
		$lines[] = 		"for(;;) {";
		$lines[] = 			$expression;
		$lines[] =			"";
		$operandSize = $this->getOperandSize($srcCount + 1);
		// group incrementations together since they can happen in parallel
		$lines[] = 			"res_ptr += $operandSize;";
		for($i = 1; $i <= $srcCount; $i++) {
			$operandSize = $this->getOperandSize($i);
			if($this->getOperandAddressMode($i) == "ARR" && $operandSize !== 0) {
				$lines[] =		"op{$i}_ptr += $operandSize;";
			}
		}
		$lines[] =			"if(res_ptr >= res_end) {";
		$lines[] =				"break;";
		$lines[] =			"}";
		for($i = 1; $i <= $srcCount; $i++) {
			$operandSize = $this->getOperandSize($i);
			if($this->getOperandAddressMode($i) == "ARR" && $operandSize !== 0) {
				$lines[] = 	"if(op{$i}_ptr >= op{$i}_end) {";
				$lines[] = 		"op{$i}_ptr = op{$i}_start;";
				$lines[] = 	"}";
			}
		}
		$lines[] = 		"}";	// end for
		$lines[] = "}"; 		// end if
		return $lines;
	}
	
	protected function isOverridden($methodName) {
		$child  = new ReflectionClass($this);
		$method = $child->getMethod($methodName);
		return ($method->class != 'Handler');
	}
}

function array_linearize($array) {
	$result = array();
	foreach($array as $element) {
		if($element !== NULL) {
			if(is_array($element)) {
				$sub_array = array_linearize($element);					
				array_splice($result, count($result), 0, $sub_array);
			} else {
				$result[] = $element;
			}
		}
	}
	return $result;
}

?>