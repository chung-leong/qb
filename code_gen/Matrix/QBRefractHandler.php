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
			$dotProductHandler = new QBDotProductHandler(NULL, $this->operandType, "VAR", $this->operandSize);
			$dotProductFunction = $dotProductHandler->getFunctionName();
		
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product, k;";
			$lines[] = "$dotProductFunction(op1_ptr, op2_ptr, MATRIX1_ROWS, MATRIX1_COLS, &dot_product);";
			$lines[] = "k = ($cType) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));";
			$lines[] = "if(k < 0.0) {";
			$lines[] = 		"for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 			"res_ptr[0] = 0.0;";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"$cType m = op3 * dot_product + qb_fast_sqrt$f(k);";
			$lines[] = 		"for(i = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 			"res_ptr[i] = op3 * op1_ptr[i] - m * op2_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);";
					$lines[] = "$cType k = ($cType) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = 0.0;";
					$lines[] = "} else {";
					$lines[] = 		"$cType m = op3 * dot_product + qb_fast_sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);";
					$lines[] = "$cType k = ($cType) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = ";
					$lines[] = 		"res_ptr[2] = 0.0;";
					$lines[] = "} else {";
					$lines[] = 		"$cType m = op3 * dot_product + qb_fast_sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = 		"res_ptr[2] = op3 * op1_ptr[2] - m * op2_ptr[2];";
					$lines[] = "}";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);";
					$lines[] = "$cType k = ($cType) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));";
					$lines[] = "if(k < 0.0) {";
					$lines[] = 		"res_ptr[0] = ";
					$lines[] = 		"res_ptr[1] = ";
					$lines[] = 		"res_ptr[2] = ";
					$lines[] = 		"res_ptr[3] = 0.0;";
					$lines[] = 	"} else {";
					$lines[] = 		"$cType m = op3 * dot_product + qb_fast_sqrt$f(k);";
					$lines[] = 		"res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];";
					$lines[] = 		"res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];";
					$lines[] = 		"res_ptr[2] = op3 * op1_ptr[2] - m * op2_ptr[2];";
					$lines[] = 		"res_ptr[3] = op3 * op1_ptr[3] - m * op2_ptr[3];";
					$lines[] = "}";
				}	break;
			}
		}
		return $lines;
		
	}
}

?>