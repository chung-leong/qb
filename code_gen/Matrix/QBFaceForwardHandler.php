<?php

class QBFaceForwardHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product;";
			$lines[] = "qb_calculate_dot_product_$type(op1_ptr, NULL, op2_ptr, NULL, MATRIX1_ROWS, &dot_product, NULL);";
			$lines[] = "if(dot_product > 0) {";
			$lines[] = 		"for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 			"res_ptr[i] = op1_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 			"res_ptr[i] = -op1_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_2x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "if(dot_product > 0) {";
					$lines[] = 		"res_ptr[0] = op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = op1_ptr[1];";
					$lines[] = "} else {";
					$lines[] = 		"res_ptr[0] = -op1_ptr[0];";
					$lines[] = 		"res_ptr[1] = -op1_ptr[1];";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_3x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
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
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_4x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
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