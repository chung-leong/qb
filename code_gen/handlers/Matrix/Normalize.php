<?php

class Normalize extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 2;
		} else {
			return 1;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "SCA";
				case 3: return "ARR";
			}
		} else {
			return parent::getOperandAddressMode($i);
		}
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op2";
				case 2: return 1;
				case 3: return "op2";
			}
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function getOperandType($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return "U32";
				case 3: return $this->operandType;
			}
		} else {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
			}
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i, vector_width = op2;";
			$lines[] = "$cType sum = 0, r;";
			$lines[] = "for(i = 0; i < vector_width; i++) {";
			$lines[] = 		"sum += op1_ptr[i] * op1_ptr[i];";
			$lines[] = "}";
			$lines[] = "if(sum) {";
			$lines[] = 		"r = qb_fast_rsqrt$f(sum);";
			$lines[] = 		"for(i = 0; i < vector_width; i++) {";
			$lines[] = 			"res_ptr[i] = op1_ptr[i] * r;";
			$lines[] = 		"}";
			$lines[] = "}";
			
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);";
					$lines[] = "if(sum) {";
					$lines[] = 		"$cType r = qb_fast_rsqrt$f(sum);";
					$lines[] = 		"res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = 		"res_ptr[1] = op1_ptr[1] * r;";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);";
					$lines[] = "if(sum) {";
					$lines[] = 		"$cType r = qb_fast_rsqrt$f(sum);";
					$lines[] = 		"res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = 		"res_ptr[1] = op1_ptr[1] * r;";
					$lines[] = 		"res_ptr[2] = op1_ptr[2] * r;";
					$lines[] = "}";
				}	break;
				case 4: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);";
					$lines[] = "if(sum) {";
					$lines[] = 		"$cType r = qb_fast_rsqrt$f(sum);";
					$lines[] = 		"res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = 		"res_ptr[1] = op1_ptr[1] * r;";
					$lines[] = 		"res_ptr[2] = op1_ptr[2] * r;";
					$lines[] = 		"res_ptr[3] = op1_ptr[3] * r;";
					$lines[] = "}";
				}	break;
			}
		}
		return $lines;
	}
}

?>