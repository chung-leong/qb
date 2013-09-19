<?php

class Distance extends Handler {

	use MultipleAddressMode, BinaryOperator, UnitResult, FloatingPointOnly;

	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 3;
		} else {
			return 2;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "SCA";
				case 4: return $this->addressMode;
			}
		} else {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return $this->addressMode;
			}
		}
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op3";
				case 2: return "op3";
				case 3: return 1;
				case 4: return "op3";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize;
				case 2: return $this->operandSize;
				case 3: return $this->operandSize;
			}
		}
	}

	public function getOperandType($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
				case 3: return "U32";
				case 4: return $this->operandType;
			}
		} else {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
				case 3: return $this->operandType;
			}
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i, vector_width = op3;";
			$lines[] = "$cType sum = 0;";
			$lines[] = "for(i = 0; i < vector_width; i++) {";
			$lines[] = 		"sum += (op1_ptr[i] - op2_ptr[i]) * (op1_ptr[i] - op2_ptr[i]);";
			$lines[] = "}";
			$lines[] = "res = qb_fast_sqrt$f(sum);";
		} else {
			switch($this->operandSize) {
				case 1: {
					$lines[] = "res = fabs$f(op1_ptr[0] - op2_ptr[0]);";
				}	break;
				case 2: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
				case 3: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
				case 4: {
					$lines[] = "$cType sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]) + (op1_ptr[3] - op2_ptr[3]) * (op1_ptr[3] - op2_ptr[3]);";
					$lines[] = "res = qb_fast_sqrt$f(sum);";
				}	break;
			}
		}
		return $lines;
	}
}

?>