<?php

class MultiplyMatrixByVector extends Handler {

	use ArrayAddressMode, BinaryOperator, MatrixConventionDependent, FloatingPointOnly, Multithreaded;

	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 4;
		} else {
			return 2;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "SCA";	// matrix rows
				case 4: return "SCA";	// matrix rols
				case 5: return "ARR";
			}
		} else {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "ARR";
			}
		}
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "(op3 * op4)";
				case 2: return "op3";
				case 3: return 1;
				case 4: return 1;
				case 5: return "op4";
			}
		} else {
			$padding = $this->getPadding();
			$matrixSize = ($this->operandSize + $padding) * $this->operandSize;
			switch($i) {
				case 1: return $matrixSize;
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
				case 4: return "U32";
				case 5: return $this->operandType;
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
		$order = $this->getMatrixConvention();
		$lines = array();
		if($this->operandSize == "variable") {
			if($order == "row-major") {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "uint32_t i, j, k;";
				$lines[] = "uint32_t matrix1_rows = op3, matrix1_cols = op4;";
				$lines[] = "$cType *buffer = do_alloca(matrix1_rows * sizeof($cType), use_heap);";
				$lines[] = "for(i = 0, k = 0; i < matrix1_rows; ++i) {";
				$lines[] = 		"$cType dot_product = 0;";
				$lines[] = 		"for(j = 0; j < matrix1_cols; ++j) {";
				$lines[] = 			"dot_product += op1_ptr[k++] * op2_ptr[j];";
				$lines[] = 		"}";
				$lines[] = 		"buffer[i] = dot_product;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, matrix1_rows * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			} else {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "uint32_t i, j, k;";
				$lines[] = "uint32_t matrix1_rows = op3, matrix1_cols = op4;";
				$lines[] = "$cType *buffer = do_alloca(matrix1_rows * sizeof($cType), use_heap);";
				$lines[] = "for(i = 0; i < matrix1_rows; ++i) {";
				$lines[] = 		"$cType dot_product = 0;";
				$lines[] = 		"for(j = 0, k = i; j < matrix1_cols; ++j, k += matrix1_rows) {";
				$lines[] = 			"dot_product += op1_ptr[k] * op2_ptr[j];";
				$lines[] = 		"}";
				$lines[] = 		"buffer[i] = dot_product;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, matrix1_rows * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			}
		} else {
			if($order == "row-major") {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[1 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
					}	break;
					case 3: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1]) + (op1_ptr[0 * 3 + 2] * op2_ptr[2]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[1 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[2 * 3 + 0] * op2_ptr[0]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
						$lines[] = "res_ptr[2] = dot_product2;";
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2]) + (op1_ptr[0 * 4 + 3] * op2_ptr[3]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[1 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[2 * 4 + 0] * op2_ptr[0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[3 * 4 + 0] * op2_ptr[0]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
						$lines[] = "res_ptr[2] = dot_product2;";
						$lines[] = "res_ptr[3] = dot_product3;";
					}	break;
				}
			} else {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);";
						$lines[] = "res_ptr[0] = dot_product0;";
						$lines[] = "res_ptr[1] = dot_product1;";
					}	break;
					case 3: {
						if($this->getPadding()) {
							$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]);";
							$lines[] = "res_ptr[0] = dot_product0;";
							$lines[] = "res_ptr[1] = dot_product1;";
							$lines[] = "res_ptr[2] = dot_product2;";
						} else {
							$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);";
							$lines[] = "res_ptr[0] = dot_product0;";
							$lines[] = "res_ptr[1] = dot_product1;";
							$lines[] = "res_ptr[2] = dot_product2;";
						}
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);";
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