<?php

// note: using column-major order

class QBMultiplyVectorByMatrixHandler extends QBMultiplyMatrixByMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "(1 * MATRIX1_COLS)";
				case 2: return "(MATRIX2_ROWS * MATRIX2_COLS)";
				case 3: return "(1 * MATRIX2_COLS)";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize;
				case 2: return ($this->operandSize + $this->operandPadding) * $this->operandSize;
				case 3: return $this->operandSize;
			}
		}
	}
		
	public function getActionForUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		if($this->operandSize == "variable") {
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
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[0 * 2 + 1]);";
					$lines[] = "$cType dot_product1 = (op1_ptr[0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);";
					$lines[] = "res_ptr[0] = dot_product0;";
					$lines[] = "res_ptr[1] = dot_product1;";
				}	break;
				case 3: {
					if($this->operandPadding) {
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
		return $lines;
	}
}

?>