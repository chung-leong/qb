<?php

class QBNormalizeHandler extends QBMatrixHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType sum = 0, r;";
			$lines[] = "for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 		"sum += op1_ptr[i] * op1_ptr[i];";
			$lines[] = "}";
			$lines[] = "r = rsqrt$f(sum);";
			$lines[] = "for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 		"res_ptr[i] = op1_ptr[i] * r;";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);";
					$lines[] = "$cType r = rsqrt$f(sum);";
					$lines[] = "res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = "res_ptr[1] = op1_ptr[1] * r;";
				}	break;
				case 3: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);";
					$lines[] = "$cType r = rsqrt$f(sum);";
					$lines[] = "res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = "res_ptr[1] = op1_ptr[1] * r;";
					$lines[] = "res_ptr[2] = op1_ptr[2] * r;";
				}	break;
				case 4: {
					$lines[] = "$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);";
					$lines[] = "$cType r = rsqrt$f(sum);";
					$lines[] = "res_ptr[0] = op1_ptr[0] * r;";
					$lines[] = "res_ptr[1] = op1_ptr[1] * r;";
					$lines[] = "res_ptr[2] = op1_ptr[2] * r;";
					$lines[] = "res_ptr[3] = op1_ptr[3] * r;";
				}	break;
			}
		}
		return $lines;
	}
}

?>