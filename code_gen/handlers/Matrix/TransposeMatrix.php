<?php

// note: not affected by matrix order

class TransposeMatrix extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly, Multithreaded;
	
	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 3;
		} else {
			return 1;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "SCA";
				case 3: return "SCA";
				case 4: return "ARR";
			}
		} else {
			return parent::getOperandAddressMode($i);
		}
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op2 * op3";
				case 2: return 1;
				case 3: return 1;
				case 4: return "op2 * op3";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize * $this->operandSize;
				case 2: return $this->operandSize * $this->operandSize;
			}
			
		}
	}
	
	public function getOperandType($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return "U32";
				case 3: return "U32";
				case 4: return $this->operandType;
			}
		} else {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
			}
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		if($this->operandSize == "variable") {
			$lines[] = "ALLOCA_FLAG(use_heap)";
			$lines[] = "uint32_t i, j, k, p;";
			$lines[] = "uint32_t matrix_rows = op2, matrix_cols = op3;";
			$lines[] = "$cType *__restrict buffer = do_alloca(matrix_rows * matrix_cols * sizeof($cType), use_heap);";
			$lines[] = "for(i = 0, p = 0; i < matrix_rows; i++) {";
			$lines[] = 		"for(j = 0, k = i; j < matrix_cols; j++, p++, k += matrix_rows) {";
			$lines[] = 			"buffer[k] = op1_ptr[p];";
			$lines[] = 		"}";
			$lines[] = "}";
			$lines[] = "memcpy(res_ptr, buffer, matrix_rows * matrix_cols * sizeof($cType));";
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