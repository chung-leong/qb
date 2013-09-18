<?php

class FaceForward extends Handler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		if($this->operandSize == "variable") {
			$dotProductHandler = new DotProduct(NULL, $this->operandType, "SCA", $this->operandSize);
			$dotProductFunction = $dotProductHandler->getFunctionName();
			
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product;";
			$lines[] = "$dotProductFunction(op1_ptr, op2_ptr, MATRIX1_ROWS, MATRIX1_COLS, &dot_product);";
			$lines[] = "if(dot_product > 0) {";
			$lines[] = 		"for(i = 0; i < MATRIX1_COLS; i++) {";
			$lines[] = 			"res_ptr[i] = op1_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"for(i = 0; i < MATRIX1_COLS; i++) {";
			$lines[] = 			"res_ptr[i] = -op1_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);";
					$lines[] = "if(dot_product > 0) {";
					$lines[] = 		"res_ptr[0] = op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = op1_ptr[1];";
					$lines[] = "} else {";
					$lines[] = 		"res_ptr[0] = -op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = -op1_ptr[1];";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);";
					$lines[] = "if(dot_product > 0) {";
					$lines[] = 		"res_ptr[0] = op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = op1_ptr[1];";
					$lines[] = 		"res_ptr[2] = op1_ptr[2];";
					$lines[] = "} else {";
					$lines[] = 		"res_ptr[0] = -op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = -op1_ptr[1];";
					$lines[] = 		"res_ptr[2] = -op1_ptr[2];";
					$lines[] = "}";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);";
					$lines[] = "if(dot_product > 0) {";
					$lines[] = 		"res_ptr[0] = op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = op1_ptr[1];";
					$lines[] = 		"res_ptr[2] = op1_ptr[2];";
					$lines[] = 		"res_ptr[3] = op1_ptr[3];";
					$lines[] = "} else {";
					$lines[] = 		"res_ptr[0] = -op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = -op1_ptr[1];";
					$lines[] = 		"res_ptr[2] = -op1_ptr[2];";
					$lines[] = 		"res_ptr[3] = -op1_ptr[3];";
					$lines[] = "}";
				}	break;
			}
		}
		return $lines;
	}
}

?>