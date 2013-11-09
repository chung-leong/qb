<?php

class ArrayResize extends Handler {

	use MainThreadExecution;
	
	protected $dimensionCount;
	
	public function __construct($baseName, $operandType, $dimensionCount) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = 1;
		$this->addressMode = "ARR";
		$this->multipleData = false;
		$this->dimensionCount = $dimensionCount;
	}
	
	public function getInputOperandCount() {
		return ($this->dimensionCount * 3 + 1);
	}
	
	public function getOutputOperandCount() {
		return 1;
	}

	public function changesOperand($i) {
		if($i <= $this->dimensionCount * 3) {
			$j = $i % 3; 
			switch($j) {
				case 2:	return true;		// destination array size
				case 0:	return true;		// destination dimension
			}
		}
		return false;
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		if($i <= $this->dimensionCount * 3) {
			$j = $i % 3; 
			switch($j) {
				case 1:	return "U32";		// source dimension
				case 2:	return "U32";		// destination array size
				case 0:	return "U32";		// destination dimension
			}
		} else {
			$j = $i - $this->dimensionCount * 3;
			switch($j) {
				case 1: return "U32";				// segment selector
				case 2: return $this->operandType;	// the array being resized
			}
		}
	}
	
	public function getOperandAddressMode($i) {
		if($i <= $this->dimensionCount * 3) {
			$j = $i % 3; 
			switch($j) {
				case 1:	return "SCA";		// source dimension
				case 2:	return "SCA";		// destination array size
				case 0:	return "SCA";		// destination dimension
			}
		} else {
			$j = $i - $this->dimensionCount * 3;
			switch($j) {
				case 1: return "CON";		// segment selector
				case 2: return "ARR";		// the array being resized
			}
		}
	}

	protected function getFunctionNameComponents($prefix) {
		$parts = parent::getFunctionNameComponents($prefix);
		array_splice($parts, -1, 0, $this->dimensionCount);
		return $parts;
	}
	
	public function getHelperFunctions() {
		$opCount = $this->getOperandCount();
		$type = $this->getOperandType($opCount);
		$cType = $this->getOperandCType($opCount);
		$functions = array(
			array(
				"void qb_relocate_elements_{$type}($cType *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count) {",
					"uint32_t i;",
					"int32_t expansion = FALSE;",
					"int32_t contraction = FALSE;",
					"uint32_t level = 0;",
					"uint32_t copy_counts[MAX_DIMENSION + 1], zero_counts[MAX_DIMENSION + 1], skip_counts[MAX_DIMENSION + 1];",
					"uint32_t remaining[MAX_DIMENSION + 1];",
					"uint32_t old_cell_index, new_cell_index = 0;",
					"uint32_t old_length = 1, new_length = 1;",
				
					"// shift index by one to simplify loop logic",
					"for(i = dimension_count; (int32_t) i > 0; i--) {",
						"uint32_t old_dim = old_dims[i - 1];",
						"uint32_t new_dim = new_dims[i - 1];",
						"if(new_dim > old_dim) {",
							"expansion = TRUE;",
							"copy_counts[i] = old_dim;",
							"zero_counts[i] = (new_dim - old_dim) * new_length;",
							"skip_counts[i] = 0;",
						"} else if(old_dim > new_dim) {",
							"contraction = TRUE;",
							"copy_counts[i] = new_dim;",
							"zero_counts[i] = 0;",
							"skip_counts[i] = (old_dim - new_dim) * old_length;",
						"} else {",
							"copy_counts[i] = old_dim;",
							"zero_counts[i] = 0;",
							"skip_counts[i] = 0;",
						"}",
						"old_length *= old_dim;",
						"new_length *= new_dim;",
					"}",
					"copy_counts[0] = 1;",
					"zero_counts[0] = 0;",
					"skip_counts[0] = 0;",
					"remaining[0] = -1;",
				
					"if(contraction) {",
						"// scan forward if some dimensions got smaller",
						"level = 0;",
						"old_cell_index = 0;",
						"new_cell_index = 0;",
						"for(;;) {",
							"while(level < dimension_count) {",
								"level++;",
								"remaining[level] = copy_counts[level];",
							"}",
							"// copy only when elements are moved backward",
							"if(new_cell_index < old_cell_index) {",
								"while(remaining[level] != 0) {",
									"elements[new_cell_index] = elements[old_cell_index];",
									"new_cell_index++;",
									"old_cell_index++;",
									"remaining[level]--;",
								"}",
							"} else {",
								"new_cell_index += remaining[level];",
								"old_cell_index += remaining[level];",
								"remaining[level] = 0;",
							"}",
							"while(remaining[level] == 0) {",
								"old_cell_index += skip_counts[level];",
								"new_cell_index += zero_counts[level];",
								"level--;",
								"remaining[level]--;",
							"}",
							"if(level == 0) {",
								"break;",
							"}",
						"}",
					"}",
					"if(expansion) {",
						"// scan backward if some dimensions got bigger",
						"level = 0;",
						"old_cell_index = old_length - 1;",
						"new_cell_index = new_length - 1;",
						"for(;;) {",
							"while(level < dimension_count) {",
								"level++;",
								"remaining[level] = copy_counts[level];",
								"old_cell_index -= skip_counts[level];",
								"// zero out new space ",
								"for(i = 0; i < zero_counts[level]; i++) {",
									"elements[new_cell_index] = 0;",
									"new_cell_index--;",
								"}",
							"}",
							"// copy only when elements are moved forward",
							"if(new_cell_index > old_cell_index) {",
								"while(remaining[level] != 0) {",
									"elements[new_cell_index] = elements[old_cell_index];",
									"new_cell_index--;",
									"old_cell_index--;",
									"remaining[level]--;",
								"}",
							"} else {",
								"new_cell_index -= remaining[level];",
								"old_cell_index -= remaining[level];",
								"remaining[level] = 0;",
							"}",
							"while(remaining[level] == 0) {",
								"level--;",
								"remaining[level]--;",
							"}",
							"if(level == 0) {",
								"break;",
							"}",
						"}",
					"}",
				"}",
			)
		);
		return $functions;
	}
	
	public function getActionOnUnitData() {
		$opCount = $this->getOperandCount();
		$type = $this->getOperandType($opCount);
		$cType = $this->getOperandCType($opCount);
		
		$changed = array();
		$newDims = array();
		$oldDims = array();
		for($i = 0, $j = 1, $k = 3; $i < $this->dimensionCount; $i++, $j += 3, $k += 3) {
			$newDims[] = "op$j";
			$oldDims[] = "op$k";
			$changed[] = "op$j != op$k";
		}
		
		$newLengths = array();
		for($i = 0, $m = 2; $i < $this->dimensionCount; $i++, $m += 3) {
			$newLengths[] = implode(" * ", array_slice($newDims, $i));
			$oldLengths[] = "op$m";
		}
		$newLength = $newLengths[0];
		$oldLength = $oldLengths[0];
		$changed = implode(" || ", $changed);
		
		$segmentSelector = "op" . ($this->dimensionCount * 3 + 1);
		$elementSize = "sizeof($cType)";
		
		$lines = array();
		$lines[] =		"if(EXPECTED($changed)) {";
		$lines[] =			"uint32_t old_dims[$this->dimensionCount] = {";
		$lines[] =				implode(", ", $oldDims);
		$lines[] =			"};";
		$lines[] =			"uint32_t new_dims[$this->dimensionCount] = {";
		$lines[] =				 implode(", ", $newDims);
		$lines[] =			"};";
		$lines[] =			"uint32_t old_length = $oldLength, new_length = $newLength;";
		$lines[] =			"";
		for($i = 0; $i < $this->dimensionCount; $i++) {
			$lines[] =		"{$oldLengths[$i]} = {$newLengths[$i]};";
			$lines[] =		"{$oldDims[$i]} = {$newDims[$i]};";
		}
		$lines[] =			"";
		$lines[] =			"if(new_length > old_length) {";
		$lines[] =				"((int8_t *) res_ptr) += qb_resize_segment(&cxt->function->local_storage->segments[$segmentSelector], new_length * $elementSize);";
		$lines[] =				"if(old_length > 0) {";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, $this->dimensionCount);";
		$lines[] =				"}";
		$lines[] =			"} else if(old_length < new_length) {";
		$lines[] =				"if(new_length > 0) {";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, $this->dimensionCount);";
		$lines[] =				"}";
		$lines[] =				"qb_resize_segment(&cxt->function->local_storage->segments[$segmentSelector], new_length * $elementSize);";
		$lines[] =			"} else {";
		$lines[] =				"((int8_t *) res_ptr) += qb_resize_segment(&cxt->function->local_storage->segments[$segmentSelector], new_length * $elementSize);";
		$lines[] =				"if(old_length > 0) {";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, $this->dimensionCount);";
		$lines[] =				"}";
		$lines[] =			"}";
		$lines[] =		"}";
		return $lines;
	}
}

?>