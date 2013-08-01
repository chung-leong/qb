<?php

class QBTransposeMatrixHandler extends QBMatrixHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			return "MATRIX1_ROWS * MATRIX1_COLS";
		} else {
			return $this->operandSize * $this->operandSize;
		}
	}
	
	public function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = $lines[] = "ALLOCA_FLAG(use_heap)";
			$lines[] = "$cType *__restrict buffer = do_alloca(MATRIX1_ROWS * MATRIX1_COLS * sizeof($cType), use_heap);";
			$lines[] = "uint32_t i, j, k, p;";
			$lines[] = "for(i = 0, p = 0; i < MATRIX1_ROWS; i++) {";
			$lines[] = 		"for(j = 0, k = i; j < MATRIX1_COLS; j++, p++, k += MATRIX1_ROWS) {";
			$lines[] = 			"buffer[k] = op1_ptr[p];";
			$lines[] = 		"}";
			$lines[] = "}";
			$lines[] = "memcpy(res_ptr, buffer, MATRIX1_ROWS * MATRIX1_COLS * sizeof($cType));";
			$lines[] = "free_alloca(buffer, use_heap);";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType m00 = op1_ptr[0 * 2 + 0];";
					$lines[] = "$cType m10 = op1_ptr[0 * 2 + 1];";
					$lines[] = "$cType m01 = op1_ptr[1 * 2 + 0];";
					$lines[] = "$cType m11 = op1_ptr[1 * 2 + 1];";
					$lines[] = "res_ptr[0 * 2 + 0] = m00;";
					$lines[] = "res_ptr[0 * 2 + 1] = m01;";
					$lines[] = "res_ptr[1 * 2 + 0] = m10;";
					$lines[] = "res_ptr[1 * 2 + 1] = m11;";
				}	break;
				case 3: {
					$lines[] = "$cType m00 = op1_ptr[0 * 3 + 0];";
					$lines[] = "$cType m10 = op1_ptr[0 * 3 + 1];";
					$lines[] = "$cType m20 = op1_ptr[0 * 3 + 2];";
					$lines[] = "$cType m01 = op1_ptr[1 * 3 + 0];";
					$lines[] = "$cType m11 = op1_ptr[1 * 3 + 1];";
					$lines[] = "$cType m21 = op1_ptr[1 * 3 + 2];";
					$lines[] = "$cType m02 = op1_ptr[2 * 3 + 0];";
					$lines[] = "$cType m12 = op1_ptr[2 * 3 + 1];";
					$lines[] = "$cType m22 = op1_ptr[2 * 3 + 2];";
					$lines[] = "res_ptr[0 * 3 + 0] = m00;";
					$lines[] = "res_ptr[0 * 3 + 1] = m01;";
					$lines[] = "res_ptr[0 * 3 + 2] = m02;";
					$lines[] = "res_ptr[1 * 3 + 0] = m10;";
					$lines[] = "res_ptr[1 * 3 + 1] = m11;";
					$lines[] = "res_ptr[1 * 3 + 2] = m12;";
					$lines[] = "res_ptr[2 * 3 + 0] = m20;";
					$lines[] = "res_ptr[2 * 3 + 1] = m21;";
					$lines[] = "res_ptr[2 * 3 + 2] = m22;";
				}	break;
				case 4: {
					$lines[] = "$cType m00 = op1_ptr[0 * 4 + 0];";
					$lines[] = "$cType m10 = op1_ptr[0 * 4 + 1];";
					$lines[] = "$cType m20 = op1_ptr[0 * 4 + 2];";
					$lines[] = "$cType m30 = op1_ptr[0 * 4 + 3];";
					$lines[] = "$cType m01 = op1_ptr[1 * 4 + 0];";
					$lines[] = "$cType m11 = op1_ptr[1 * 4 + 1];";
					$lines[] = "$cType m21 = op1_ptr[1 * 4 + 2];";
					$lines[] = "$cType m31 = op1_ptr[1 * 4 + 3];";
					$lines[] = "$cType m02 = op1_ptr[2 * 4 + 0];";
					$lines[] = "$cType m12 = op1_ptr[2 * 4 + 1];";
					$lines[] = "$cType m22 = op1_ptr[2 * 4 + 2];";
					$lines[] = "$cType m32 = op1_ptr[2 * 4 + 3];";
					$lines[] = "$cType m03 = op1_ptr[3 * 4 + 0];";
					$lines[] = "$cType m13 = op1_ptr[3 * 4 + 1];";
					$lines[] = "$cType m23 = op1_ptr[3 * 4 + 2];";
					$lines[] = "$cType m33 = op1_ptr[3 * 4 + 3];";
					$lines[] = "res_ptr[0 * 4 + 0] = m00;";
					$lines[] = "res_ptr[0 * 4 + 1] = m01;";
					$lines[] = "res_ptr[0 * 4 + 2] = m02;";
					$lines[] = "res_ptr[0 * 4 + 3] = m03;";
					$lines[] = "res_ptr[1 * 4 + 0] = m10;";
					$lines[] = "res_ptr[1 * 4 + 1] = m11;";
					$lines[] = "res_ptr[1 * 4 + 2] = m12;";
					$lines[] = "res_ptr[1 * 4 + 3] = m13;";
					$lines[] = "res_ptr[2 * 4 + 0] = m20;";
					$lines[] = "res_ptr[2 * 4 + 1] = m21;";
					$lines[] = "res_ptr[2 * 4 + 2] = m22;";
					$lines[] = "res_ptr[2 * 4 + 3] = m23;";
					$lines[] = "res_ptr[3 * 4 + 0] = m30;";
					$lines[] = "res_ptr[3 * 4 + 1] = m31;";
					$lines[] = "res_ptr[3 * 4 + 2] = m32;";
					$lines[] = "res_ptr[3 * 4 + 3] = m33;";
				}	break;
			}
		}
		return $lines;
	}
}

?>