<?php

class Refract extends Handler {

	use ArrayAddressMode, TernaryOperator, FloatingPointOnly;

	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 4;
		} else {
			return parent::getInputOperandCount();
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "SCA";
				case 4: return "SCA";
				case 5: return "ARR";
			}
		} else {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "SCA";
				case 4: return "ARR";
			}
		}
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op4";
				case 2: return "op4";
				case 3: return 1;
				case 4: return 1;
				case 5: return "op4";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize;
				case 2: return $this->operandSize;
				case 3: return 1;
				case 4: return $this->operandSize;
			}
		}
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(4);
		$type = $this->getOperandType(4);
		$f = ($type == 'F32') ? 'f' : '';
		if($this->operandSize == "variable") {
			$dotProductHandler = new DotProduct(NULL, $this->operandType, "SCA", $this->operandSize);
			$dotProductFunction = $dotProductHandler->getHandlerFunctionName();
		
			$lines[] = "uint32_t i, vector_width = op4;";
			$lines[] = "$cType dot_product, k;";
			$lines[] = "$dotProductFunction(op1_ptr, op2_ptr, vector_width, vector_width, &dot_product);";
			$lines[] = "k = ($cType) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));";
			$lines[] = "if(k < 0.0) {";
			$lines[] = 		"for(i = 0; i < vector_width; i++) {";
			$lines[] = 			"res_ptr[0] = 0.0;";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"$cType m = op3 * dot_product + qb_fast_sqrt$f(k);";
			$lines[] = 		"for(i = 0; i < vector_width; i++) {";
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