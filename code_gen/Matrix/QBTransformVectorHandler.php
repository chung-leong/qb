<?php

// note: using row-major order

class QBTransformVectorHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: return ($this->operandSize + 1) * $this->operandSize;
			case 2: return $this->operandSize;
			case 3: return $this->operandSize;
		}
	}
	
	public function getResultSizePossibilities() {
		return $this->operandSize;
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		if($this->order == "row-major") {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1]) + op1_ptr[0 * 3 + 2];";
					$lines[] = "$cType dot_product1 = (op1_ptr[1 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + op1_ptr[1 * 3 + 2];";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2]) + op1_ptr[0 * 4 + 3];";
					$lines[] = "$cType dot_product1 = (op1_ptr[1 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2]) + op1_ptr[1 * 4 + 3];";
					$lines[] = "$cType dot_product2 = (op1_ptr[2 * 4 + 0] * op2_ptr[0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + op1_ptr[2 * 4 + 3];";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
					$lines[] = "res_ptr[2] = dot_product2;";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 5 + 0] * op2_ptr[0]) + (op1_ptr[0 * 5 + 1] * op2_ptr[1]) + (op1_ptr[0 * 5 + 2] * op2_ptr[2]) + (op1_ptr[0 * 5 + 3] * op2_ptr[3]) + op1_ptr[0 * 5 + 4];";
					$lines[] = "$cType dot_product1 = (op1_ptr[1 * 5 + 0] * op2_ptr[0]) + (op1_ptr[1 * 5 + 1] * op2_ptr[1]) + (op1_ptr[1 * 5 + 2] * op2_ptr[2]) + (op1_ptr[1 * 5 + 3] * op2_ptr[3]) + op1_ptr[1 * 5 + 4];";
					$lines[] = "$cType dot_product2 = (op1_ptr[2 * 5 + 0] * op2_ptr[0]) + (op1_ptr[2 * 5 + 1] * op2_ptr[1]) + (op1_ptr[2 * 5 + 2] * op2_ptr[2]) + (op1_ptr[2 * 5 + 3] * op2_ptr[3]) + op1_ptr[2 * 5 + 4];";
					$lines[] = "$cType dot_product3 = (op1_ptr[3 * 5 + 0] * op2_ptr[0]) + (op1_ptr[3 * 5 + 1] * op2_ptr[1]) + (op1_ptr[3 * 5 + 2] * op2_ptr[2]) + (op1_ptr[3 * 5 + 3] * op2_ptr[3]) + op1_ptr[3 * 5 + 4];";				
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
					$lines[] = "res_ptr[2] = dot_product2;";
					$lines[] = "res_ptr[3] = dot_product3;";
				}	break;
			}
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]) + op1_ptr[2 * 2 + 0];";
					$lines[] = "$cType dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]) + op1_ptr[2 * 2 + 1];";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]) + op1_ptr[3 * 3 + 0];";
					$lines[] = "$cType dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]) + op1_ptr[3 * 3 + 1];";
					$lines[] = "$cType dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]) + op1_ptr[3 * 3 + 2];";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
					$lines[] = "res_ptr[2] = dot_product2;";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]) + op1_ptr[4 * 4 + 0];";
					$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]) + op1_ptr[4 * 4 + 1];";
					$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]) + op1_ptr[4 * 4 + 2];";
					$lines[] = "$cType dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]) + op1_ptr[4 * 4 + 3];";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
					$lines[] = "res_ptr[2] = dot_product2;";
					$lines[] = "res_ptr[3] = dot_product3;";
				}	break;
			}
		}
		return $lines;
	}
}

?>