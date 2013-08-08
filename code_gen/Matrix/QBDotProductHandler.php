<?php

class QBDotProductHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: 
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}
		
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "expr_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$vectorSize = $this->getOperandSize(1);
			return "expr_count = ((op1_count > op2_count) ? op1_count : op2_count) / $vectorSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 3) {
			return 1;	// the result
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function needsMatrixDimensions($which = null) {
		if($this->operandSize == "variable") {
			if(!$which || $which == 1) {
				// only need the first, since the second must have the same size
				return true;
			}
		}
		return false;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType sum = 0;";
			$lines[] = "for(i = 0; i < MATRIX1_COLS; i++) {";
			$lines[] = 		"sum += op1_ptr[i] * op2_ptr[i];";
			$lines[] = "}";
			$lines[] = "res = sum;";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "res = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);";
				}	break;
				case 3: {
					$lines[] = "res = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);";
				}	break;
				case 4: {
					$lines[] = "res = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);";
				}	break;
			}
		}
		return $lines;
	}
}

?>