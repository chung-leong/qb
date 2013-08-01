<?php

class QBDistanceHandler extends QBMatrixHandler {

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
			return "vector_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$vectorSize = $this->getOperandSize(1);
			return "vector_count = ((op1_count > op2_count) ? op1_count : op2_count) / $vectorSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 3) {
			return 1;	// the result
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function getActionForUnitData() {
		$type = $this->getOperandType(3);
		$cType = $this->getOperandCType(3);
		$f = ($type == 'F32') ? 'f' : '';
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType sum = 0;";
			$lines[] = "for(i = 0; i < MATRIX2_ROWS; i++) {";
			$lines[] = 		"sum += (op1_ptr[i] - op2_ptr[i]) * (op1_ptr[i] - op2_ptr[i]);";
			$lines[] = "}";
			$lines[] = "res = sqrt$f(sum);";
		} else {
			switch($this->operandSize) {
				case 1: {
					$lines[] = "res = fabs$f(op1_ptr[0] - op2_ptr[0]);";
				}	break;
				case 2: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]);";
					$lines[] = "res = sqrt$f(sum);";
				}	break;
				case 3: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]);";
					$lines[] = "res = sqrt$f(sum);";
				}	break;
				case 4: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]) + (op1_ptr[3] - op2_ptr[3]) * (op1_ptr[3] - op2_ptr[3]);";
					$lines[] = "res = sqrt$f(sum);";
				}	break;
			}
		}
		return $lines;
	}
}

?>