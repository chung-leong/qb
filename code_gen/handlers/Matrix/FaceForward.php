<?php

class FaceForward extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly;

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
		$cType = $this->getOperandCType(1);
		$lines = array();
		if($this->operandSize == "variable") {
			$dotProductHandler = new DotProduct(NULL, $this->operandType, "SCA", $this->operandSize);
			$dotProductFunction = $dotProductHandler->getHandlerFunctionName();
			
			$lines[] = "uint32_t i, vector_width = op3;";
			$lines[] = "$cType dot_product;";
			$lines[] = "$dotProductFunction(op1_ptr, op1_count, op2_ptr, op2_count, vector_width, &dot_product);";
			$lines[] = "if(dot_product > 0) {";
			$lines[] = 		"for(i = 0; i < vector_width; i++) {";
			$lines[] = 			"res_ptr[i] = op1_ptr[i];";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"for(i = 0; i < vector_width; i++) {";
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