<?php

class QBIssetHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return ($i == 2) ? "VAR" : $this->addressMode;
	}

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getAction() {
		if($this->addressMode == "VAR") {
			return "res = (op1 != 0);";
		} else {
			return "res = (op1_ptr) && (op1 != 0);";
		}
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
			die("Invalid address mode for operand $i in $className: $addressMode\n");
		}
		$lines[] = "";
		return $lines;
	}
	
}

?>