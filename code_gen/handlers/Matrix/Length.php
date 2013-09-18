<?php

class Length extends Handler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
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
			return "vector_count = op1_count / $vectorSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 2) {
			return 1;
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType sum = 0;";
			$lines[] = "for(i = 0; i < MATRIX1_COLS; i++) {";
			$lines[] = 		"sum += op1_ptr[i] * op1_ptr[i];";
			$lines[] = "}";
			$lines[] = "res = qb_fast_sqrt$f(sum);";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
				case 3: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
				case 4: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
			}
		}
		return $lines;
	}
}

?>