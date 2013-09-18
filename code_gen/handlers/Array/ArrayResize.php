<?php

class ArrayResize extends Handler {

	public function getInputOperandCount() {
		return null;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			default: return "U32";
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			default: return "SCA";
		}
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_relocate_elements_{$type}($cType *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count) {",
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
	
	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$instr = $this->getInstructionStructure();
		$lines[] = $this->getLabelCode($name);
		$lines[] = $this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler");
		$lines[] = "{";
		$lines[] =		"uint32_t dimension_count = (($instr *) instruction_pointer)->argument_count;";
		$lines[] = 		"uint32_t *operands = (($instr *) instruction_pointer)->operands;";
		$lines[] =		"uint32_t old_dims[MAX_DIMENSION], new_dims[MAX_DIMENSION];";
		$lines[] =		"uint32_t *dim_ptr, *new_dim_ptr, *size_ptr;";
		$lines[] =		"uint32_t old_length, new_length = 1, i;";
		$lines[] =		"int32_t changed = FALSE;";
		$lines[] = 		"$cType *res_ptr;";
		$lines[] =		"";
		$lines[] = 		"selector = (($instr *) instruction_pointer)->operand1 & 0x00FF;";
		$lines[] = 		"index_index = ((($instr *) instruction_pointer)->operand1 >> 8) & 0x03FF;";
		$lines[] = 		"size_index = (($instr *) instruction_pointer)->operand1 >> 20;";
		$lines[] = 		"res_start_index = ((uint32_t *) segment0)[index_index];";
		$lines[] = 		"res_count = ((uint32_t *) segment0)[size_index];";				
		$lines[] =		"old_length = res_count;";
		$lines[] =		"for(i = dimension_count - 1; (int32_t) i >= 0; i--) {";
		$lines[] =			"size_ptr = ((uint32_t *) segment0) + operands[i * 3 + 0];";
		$lines[] = 			"dim_ptr = ((uint32_t *) segment0) + operands[i * 3 + 1];";
		$lines[] = 			"new_dim_ptr = ((uint32_t *) segment0) + operands[i * 3 + 2];";
		$lines[] =			"old_dims[i] = *dim_ptr;";
		$lines[] =			"new_dims[i] = *new_dim_ptr;";
		$lines[] = 			"if(*dim_ptr != *new_dim_ptr) {";
		$lines[] = 				"changed = TRUE;";
		$lines[] =				"*dim_ptr = *new_dim_ptr;";
		$lines[] = 			"}";
		$lines[] =			"new_length *= *new_dim_ptr;";
		$lines[] =			"*size_ptr = new_length;";
		$lines[] = 		"}";
		$lines[] =		"if(EXPECTED(changed)) {";
		$lines[] =			"if(new_length > old_length) {";
		$lines[] = 				"qb_enlarge_segment(cxt, &cxt->storage->segments[selector], new_length);";
		$lines[] =				"if(old_length > 0) {";
		$lines[] = 					"res_ptr = (($cType *) segments[selector]) + res_start_index;";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, dimension_count);";
		$lines[] =				"}";
		$lines[] =			"} else if(old_length < new_length) {";
		$lines[] =				"if(new_length > 0) {";
		$lines[] = 					"res_ptr = (($cType *) segments[selector]) + res_start_index;";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, dimension_count);";
		$lines[] =				"}";
		$lines[] =				"qb_shrink_segment(cxt, &cxt->storage->segments[selector], 0, new_length);";
		$lines[] =			"} else {";
		$lines[] =				"if(old_length > 0) {";
		$lines[] = 					"res_ptr = (($cType *) segments[selector]) + res_start_index;";
		$lines[] =					"qb_relocate_elements_{$type}(res_ptr, old_dims, new_dims, dimension_count);";
		$lines[] =				"}";
		$lines[] =			"}";
		$lines[] =		"}";
		$lines[] =		"instruction_pointer += sizeof((($instr *) instruction_pointer)->next_handler) + (($instr *) instruction_pointer)->operand_size;";
		$lines[] = "}";
		$lines[] = $this->getJumpCode();
		return $lines;
	}
	
	public function isVariableLength() {
		return true;
	}

	public function needsLineNumber($where = null) {
		return false;
	}
	
	public function getInstructionStructure() {		
		return "qb_instruction_array_resize";
	}
	
	public function getInstructionStructureDefinition() {
		$instr = $this->getInstructionStructure();
		$targetCount = $this->getJumpTargetCount();
		$opCount = $this->getOperandCount();
		$lines = array();
		$lines[] = "typedef struct $instr {";
		$lines[] = "void *next_handler;";
		$lines[] = "uint16_t operand_size;";
		$lines[] = "uint16_t argument_count;";
		$lines[] = "uint32_t operand1;";
		$lines[] = "uint32_t operands[1];";
		$lines[] = "} $instr;";
		return $lines;
	}
}

?>