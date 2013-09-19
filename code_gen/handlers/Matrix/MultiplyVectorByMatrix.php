<?php

class MultiplyVectorByMatrix extends Handler {

	use ArrayAddressMode, BinaryOperator, MatrixConventionDependent, FloatingPointOnly;

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
			return parent::getOperandAddressMode($i);
		}
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op4";
				case 2: return "(op3 * op4)";
				case 3: return 1;
				case 4: return 1;
				case 5: return "op3";
			}
		} else {
			return $this->operandSize;
		}
	}
		
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$order = $this->getMatrixConvention();
		$lines = array();
		if($this->operandSize == "variable") {
			if($order == "row-major") {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "$cType *buffer = do_alloca(MATRIX2_ROWS * sizeof($cType), use_heap);";
				$lines[] = "uint32_t i, j, k;";
				$lines[] = "for(i = 0, k = 0; i < MATRIX2_COLS; ++i) {";
				$lines[] = 		"$cType dot_product = 0;";
				$lines[] = 		"for(j = 0; j < MATRIX2_ROWS; ++j, ++k) {";
				$lines[] =			"dot_product += op1_ptr[j] * op2_ptr[k + i];";
				$lines[] =			"k += MATRIX2_COLS;";
				$lines[] = 		"}";
				$lines[] = 		"buffer[i] = dot_product;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, MATRIX2_COLS * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			} else {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "$cType *buffer = do_alloca(MATRIX2_ROWS * sizeof($cType), use_heap);";
				$lines[] = "uint32_t i, j, k;";
				$lines[] = "for(i = 0, k = 0; i < MATRIX2_COLS; ++i) {";
				$lines[] = 		"$cType dot_product = 0;";
				$lines[] = 		"for(j = 0; j < MATRIX2_ROWS; ++j, ++k) {";
				$lines[] = 			"dot_product += op1_ptr[j] * op2_ptr[k];";
				$lines[] = 		"}";
				$lines[] = 		"buffer[i] = dot_product;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, MATRIX2_COLS * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			}
		} else {
			if($order == "row-major") {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 0]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
					}	break;
					case 3: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[2] * op2_ptr[2 * 3 + 0]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2] * op2_ptr[2 * 3 + 1]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0] * op2_ptr[0 * 3 + 2]) + (op1_ptr[1] * op2_ptr[1 * 3 + 2]) + (op1_ptr[2] * op2_ptr[2 * 3 + 2]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
						$lines[] = "res_ptr[2] = dot_product2;";
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[3] * op2_ptr[3 * 4 + 0]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[3] * op2_ptr[3 * 4 + 1]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3] * op2_ptr[3 * 4 + 2]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0] * op2_ptr[0 * 4 + 3]) + (op1_ptr[1] * op2_ptr[1 * 4 + 3]) + (op1_ptr[2] * op2_ptr[2 * 4 + 3]) + (op1_ptr[3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
						$lines[] = "res_ptr[2] = dot_product2;";
						$lines[] = "res_ptr[3] = dot_product3;";
					}	break;
				}
			} else {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[0 * 2 + 1]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
					}	break;
					case 3: {
						if($this->getPadding()) {
							$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]);";
							$lines[] = "res_ptr[0] = dot_product0;";
							$lines[] = "res_ptr[1] = dot_product1;";
							$lines[] = "res_ptr[2] = dot_product2;";
						} else {
							$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2] * op2_ptr[0 * 3 + 2]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2] * op2_ptr[1 * 3 + 2]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2] * op2_ptr[2 * 3 + 2]);";
							$lines[] = "res_ptr[0] = dot_product0;";
							$lines[] = "res_ptr[1] = dot_product1;";
							$lines[] = "res_ptr[2] = dot_product2;";
						}
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3] * op2_ptr[0 * 4 + 3]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3] * op2_ptr[1 * 4 + 3]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3] * op2_ptr[2 * 4 + 3]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
						$lines[] = "res_ptr[2] = dot_product2;";
						$lines[] = "res_ptr[3] = dot_product3;";
					}	break;
				}
			}
		}
		return $lines;
	}
}

?>