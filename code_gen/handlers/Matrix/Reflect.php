<?php

class Reflect extends Handler {

	public function getInputOperandCount() {
		return 2;
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		if($this->operandSize == "variable") {
			$dotProductHandler = new DotProduct(NULL, $this->operandType, "SCA", $this->operandSize);
			$dotProductFunction = $dotProductHandler->getFunctionName();
			
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product;";
			$lines[] = "$dotProductFunction(op1_ptr, op2_ptr, MATRIX1_ROWS, MATRIX1_COLS, &dot_product);";
			$lines[] = "for(i = 0; i < MATRIX1_COLS; i++) {";
			$lines[] = 		"res_ptr[i] = ($cType) (op1_ptr[i] - 2.0 * dot_product * op2_ptr[i]);";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
					$lines[] = "res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
					$lines[] = "res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);";
					$lines[] = "res_ptr[3] = ($cType) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);";
				}	break;
			}
		}
		return $lines;
	}
}

?>