<?php

class QBHandler {
	protected static $typeDecls = array();
	protected static $compiler;

	protected $baseName;
	protected $operandType;
	protected $operandSize;
	protected $addressMode;
	
	static public function getTypeDeclarations() {
		ksort(self::$typeDecls);
		return self::$typeDecls;
	}
	
	static public function setCompiler($compiler) {
		self::$compiler = $compiler;
	}
	
	public function __construct($baseName, $operandType = NULL, $addressMode = NULL, $vectorWidth = null) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = ($vectorWidth) ? $vectorWidth : 1;
		$this->addressMode = $addressMode;
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
		$opCount = $this->getOperandCount();
		$lines[] = $this->getLabelCode($name);
		$lines[] = $this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler");
		$lines[] = "{";
		if($this->needsLineNumber()) {
			$lines[] = "#define PHP_LINE_NUMBER	(($instr *) instruction_pointer)->line_number";
		}
		if($this->needsMatrixDimensions(1)) {
			$lines[] = "#define MATRIX1_ROWS			((($instr *) instruction_pointer)->matrix_dimensions >> 20)";
			$lines[] = "#define MATRIX1_COLS			(((($instr *) instruction_pointer)->matrix_dimensions >> 10) & 0x03FF)";
		}
		if($this->needsMatrixDimensions(2)) {
			$lines[] = "#define MATRIX2_ROWS			MATRIX1_COLS";
			$lines[] = "#define MATRIX2_COLS			((($instr *) instruction_pointer)->matrix_dimensions & 0x03FF)";
		}
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandDeclaration($i);
		}
		$lines[] = "";
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		$lines[] = $action;
		if($this->needsLineNumber()) {
			$lines[] = "#undef PHP_LINE_NUMBER";
		}
		if($this->needsMatrixDimensions(1)) {
			$lines[] = "#undef MATRIX1_ROWS";
			$lines[] = "#undef MATRIX1_COLS";
		}
		if($this->needsMatrixDimensions(2)) {
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
		$opCount = $this->getOperandCount();
		$targetCount = $this->getJumpTargetCount();
		$instr = "qb_instruction";
		if($targetCount == 2) {
			$instr .= "_branch";
		} else if($targetCount == 1) {
			$instr .= "_jump";
		}
		$instr .= "_{$opCount}";
		if($this->needsMatrixDimensions()) {
			$instr .= "_matrix";
		}
		if($this->needsLineNumber()) {
			$instr .= "_lineno";
		}
		return $instr;
	}
	
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
			$lines[] = "uint32_t operand{$i};";
		}
		if($this->needsMatrixDimensions()) {
			$lines[] = "uint32_t matrix_dimensions;";
		}
		if($this->needsLineNumber()) {
			$lines[] = "uint32_t line_number;";
		}
		$lines[] = "} $instr;";
		return $lines;
	}
	
	// return the number of input operands
	public function getInputOperandCount() {
		return 1;
	}
	
	// return the number of output operands 
	public function getOutputOperandCount() {
		return 1;
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
		if($this->addressMode) {
			return $this->addressMode;
		} else {
			if($this->operandSize > 1) {
				return "ARR";
			} else {
				throw new Exception("Invalid address mode");
			}
		}
	}
	
	public function isMultipleData() {
		return ($this->addressMode == "ARR");
	}	
	
	public function isVectorized() {
		return ($this->operandSize != 1);
	}
	
	public function isVariableLength() {
		return false;
	}
	
	public function needsMatrixDimensions($which = null) {
		return false;
	}

	public function needsInterpreterContext() {
		return false;
	}
	
	public function needsOriginalSize() {
		return false;
	}
	
	public function hasAlternativeAddressModes() {
		return ($this->addressMode == 'VAR');
	}

	public function needsLineNumber($where = null) {
		if(!$where || $where == "retrieval") {
			$opCount = $this->getOperandCount();
			for($i = 1; $i <= $opCount; $i++) {
				if($this->getOperandAddressMode($i) != "VAR") {
					return true;
				}
			}
		}
		return false;
	}
		
	public function needsUnrolling() {
		return $this->isVectorized();
	}
	
	public function getFunctionName() {
		$className = get_class($this);
		$opName = substr($className, 2, -7);
		$opName = preg_replace("/([a-z])([A-Z])/", "$1_$2", $opName);
		$opName = strtolower($opName);
		$name = "qb_do_{$opName}";
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

	public function getFunctionParameters($includeType) {
		$variables = array();
		
		// see if the interpretator context is needed
		if($this->needsInterpreterContext()) {
			$variables[] = (($includeType) ? "qb_interpreter_context *" : "") . "cxt";
		}
		
		// add input operands
		$srcCount = $this->getInputOperandCount();
		for($i = 1; $i <= $srcCount; $i++) {
			$cType = $this->getOperandCType($i);
			$addressMode = $this->getOperandAddressMode($i);
			$variables[] = (($includeType) ? "$cType * __restrict " : "") . "op{$i}_ptr";
			if($addressMode == "ARR" && (!$this->isVectorized() || $this->isMultipleData())) {
				$variables[] = (($includeType) ? "uint32_t " : "") . "op{$i}_count";
			}
		}
		
		// see if matrix dimensions are needed
		if($this->needsMatrixDimensions(1)) {
			$variables[] = (($includeType) ? "uint32_t " : "") . "MATRIX1_ROWS";
			$variables[] = (($includeType) ? "uint32_t " : "") . "MATRIX1_COLS";
		}
		if($this->needsMatrixDimensions(2)) {
			$variables[] = (($includeType) ? "uint32_t " : "") . "MATRIX2_ROWS";
			$variables[] = (($includeType) ? "uint32_t " : "") . "MATRIX2_COLS";
		}
		
		// add output operand		
		$dstCount = $this->getOutputOperandCount();
		if($dstCount > 0) {
			$cType = $this->getOperandCType($srcCount + 1);
			$addressMode = $this->getOperandAddressMode($srcCount + 1);
			$variables[] = (($includeType) ? "$cType * __restrict " : "") . "res_ptr";
			if($addressMode == "ARR" && (!$this->isVectorized() || $this->isMultipleData())) {
				$variables[] = (($includeType) ? "uint32_t " : "") . "res_count";
			}
		}
		
		// see if the original size is needed
		if($this->needsOriginalSize()) {
			$variables[] = (($includeType) ? "uint32_t " : "") . "res_count_before";
		}
		
		if($this->needsLineNumber("function")) {
			$variables[] = (($includeType) ? "uint32_t " : "") . "PHP_LINE_NUMBER";
		}
		
		return $variables;
	}
	
	public function getFunctionType() {
		if(!$this->isOverridden('getAction') && !$this->isOverridden('getCode')) {
			if($this->isMultipleData()) {
				return 'extern';
			} else {
				$action = $this->getActionOnUnitData();
				$count = count($action, true);
				if($count > 1) {
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
		}
		return null;
	}
	
	public function getFunctionDefinition() {
		$functionType = $this->getFunctionType();
		if($functionType) {
			$function = $this->getFunctionName();
			$parameters = $this->getFunctionParameters(true);
			$parameterList = implode(", ", $parameters);
			$expressions = $this->getActionExpressions();
			switch($functionType) {
				case 'inline': $typeDecl = "static zend_always_inline void"; break;
				case 'extern': $typeDecl = "void ZEND_FASTCALL"; break;
			}
			$lines = array();
			$lines[] = "$typeDecl $function($parameterList) {";
			$lines[] = $expressions;
			$lines[] = "}";
			return $lines;
		}
		return null;
	}
	
	// return codes that perform what the op is supposed to do
	public function getAction() {
		$functionType = $this->getFunctionType();
		if($functionType) {	
			$function = $this->getFunctionName();
			$parameters = $this->getFunctionParameters(false);
			$parameterList = implode(", ", $parameters);
			$lines = array();
			$lines[] = "$function($parameterList);";
			return $lines;
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
				$this->addressMode = ($this->operandSize > 1) ? null : "VAR";
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
	
	// return the variable declaration needed for retrieving operand $i
	// this method is needed mainly because of the limitations of Visual C  
	protected function getOperandDeclaration($i) {
		$cType = $this->getOperandCType($i);
		$srcCount = $this->getInputOperandCount();
		$lines = array();
		if($i <= $srcCount) {
			$lines[] = "$cType *__restrict op{$i}_ptr;";
		} else {
			$lines[] = "$cType *__restrict res_ptr;";
		}
		return $lines;
	}
	
	// return code for retrieving operand $i
	// variables employed here should be declared in getOperandDeclaration() 
	protected function getOperandRetrievalCode($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		$srcCount = $this->getInputOperandCount();
		if($addressMode == "VAR") {
			$lines[] = "index = (($instr *) instruction_pointer)->operand{$i};";
			if($i <= $srcCount) {
				$lines[] = "op{$i}_ptr = (($cType *) segment0) + index;";
			} else {
				$j = $i - $srcCount;
				$lines[] = "res_ptr = (($cType *) segment0) + index;";
			}
		} else if($addressMode == "ELV") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_selector = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x00FF;";
			$lines[] = "index_index = (($instr *) instruction_pointer)->operand{$i} >> 16;";
			$lines[] = "index = ((uint32_t *) segments[index_selector])[index_index];";
			
			if($i <= $srcCount) {				
				// abort on out-of-bound
				$lines[] = "if(UNEXPECTED(index >= segment_element_counts[selector])) {";
				$lines[] = 		"qb_abort_range_error(cxt, &cxt->storage->segments[selector], index, 1, PHP_LINE_NUMBER);";
				$lines[] = "}";
				$lines[] = "op{$i}_ptr = (($cType *) segments[selector]) + index;";				
			} else {
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
			}
		} else if($addressMode == "ARR") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x03FF;";
			$lines[] = "size_index = (($instr *) instruction_pointer)->operand{$i} >> 20;";
			if($i <= $srcCount) {
				$lines[] = "op{$i}_start_index = ((uint32_t *) segment0)[index_index];";
				$lines[] = "op{$i}_count = ((uint32_t *) segment0)[size_index];";
				$lines[] = "if(UNEXPECTED(op{$i}_start_index + op{$i}_count > segment_element_counts[selector] || op{$i}_start_index + op{$i}_count < op{$i}_start_index)) {";
				$lines[] = 		"qb_abort_range_error(cxt, &cxt->storage->segments[selector], op{$i}_start_index, op{$i}_count, PHP_LINE_NUMBER);";
				$lines[] = "}";
				$lines[] = "op{$i}_ptr = (($cType *) segments[selector]) + op{$i}_start_index;";
			} else {
				$lines[] = "res_start_index = ((uint32_t *) segment0)[index_index];";
				$lines[] = "res_count = res_count_before = ((uint32_t *) segment0)[size_index];";				
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
				$lines[] = "res_ptr = (($cType *) segments[selector]) + res_start_index;";
			}
		} else {
			$className = get_class($this);
			die("Invalid address mode for operand $i in $className: $addressMode\n");
		}
		$lines[] = "";
		return $lines;
	}
	
	// return list of expressions that could be the new result size (if it's bigger)
	// the default implementation returns the sizes of all array operands
	public function getResultSizePossibilities() {
		$srcCount = $this->getInputOperandCount();
		$dstCount = $this->getOutputOperandCount();		
		if($dstCount > 0) {
			$dstMode = $this->getOperandAddressMode($srcCount + 1);
			if($dstMode == "ARR") {
				$list = array();
				for($i = 1; $i <= $srcCount; $i++) {
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
		return ($method->class != 'QBHandler');
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