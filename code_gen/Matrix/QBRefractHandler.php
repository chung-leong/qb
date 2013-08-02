<?php

class QBRefractHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 3: return "VAR";
			default: return "ARR";
		}
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(4);
		$type = $this->getOperandType(4);
		$f = ($type == 'F32') ? 'f' : '';
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product, k;";
			$lines[] = "qb_calculate_dot_product_$type(op1_ptr, NULL, op2_ptr, NULL, MATRIX2_ROWS, &dot_product, NULL);";
			$lines[] = "k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));";
			$lines[] = "if(k < 0.0) {";
			$lines[] = 		"for(i = 0; i < MATRIX2_ROWS; i++) {";
			$lines[] = 			"res_ptr[0] = 0.0;";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"$cType m = eta * dot_product + sqrt$f(k);";
			$lines[] = 		"for(i = 0; i < MATRIX2_ROWS; i++) {";
			$lines[] = 			"res_ptr[i] = eta * op1_ptr[i] - m * op2_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product, k;";
					$lines[] = "qb_calculate_dot_product_2x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = 0.0;";
					$lines[] = "} else {";
					$lines[] = 		"$cType m = eta * dot_product + sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product, k;";
					$lines[] = "qb_calculate_dot_product_3x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = ";
					$lines[] = 		"res_ptr[2] = 0.0;";
					$lines[] = "} else {";
					$lines[] = 		"$cType m = eta * dot_product + sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = 		"res_ptr[2] = eta * op1_ptr[2] - m * op2_ptr[2];";
					$lines[] = "}";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product, k;";
					$lines[] = "qb_calculate_dot_product_4x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = ";
					$lines[] = 		"res_ptr[2] = ";
					$lines[] = 		"res_ptr[3] = 0.0;";
					$lines[] = 	"} else {";
					$lines[] = 		"$cType m = eta * dot_product + sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = 		"res_ptr[2] = eta * op1_ptr[2] - m * op2_ptr[2];";
					$lines[] = 		"res_ptr[3] = eta * op1_ptr[3] - m * op2_ptr[3];";
					$lines[] = "}";
				}	break;
			}
		}
		
	}
}

?>