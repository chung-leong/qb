<?php

class MultiplyMatrixByMatrix extends Handler {

	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 6;
		} else {
			return 2;
		}
	}
	
	public function getOperandType($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return "U32";
				case 3: return "U32";
				case 4: return $this->operandType;
				case 5: return "U32";
				case 6: return "U32";
				case 7: return $this->operandType;
			}
		} else {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
				case 3: return $this->operandType;
			}
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "SCA";			//	row count
				case 3: return "SCA";			// 	col count
				case 4: return "ARR";
				case 5: return "SCA";			//	row count
				case 6: return "SCA";			//	col count
				case 7: return "ARR";
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
				case 1: return "(op2 * op3)";
				case 4: return "(op5 * op6)";
				case 7: return "(op2 * op6)";
			}
		} else {
			switch($i) {
				case 1: return ($this->operandSize + $this->operandPadding) * $this->operandSize;
				case 2: return $this->operandSize;
				case 3: return $this->operandSize;
			}
		}
	}

	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "mmult_res_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$lines = array();
			$m1 = 1;
			$m2 = ($this->operandSize == "variable") ? 4 : 2;
			$res = ($this->operandSize == "variable") ? 7 : 3;
			$matrixSize1 = $this->getOperandSize($m1); 
			$matrixSize2 = $this->getOperandSize($m2);
			$outputSize = $this->getOperandSize($res);
			$lines[] = "matrix{$m1}_count = op{$m1}_count / $matrixSize1;";
			$lines[] = "matrix{$m2}_count = op{$m2}_count / $matrixSize2;";
			$lines[] = "mmult_res_count = ((matrix1_count > matrix2_count) ? matrix1_count : matrix2_count) * $outputSize;";
			return $lines;
		}
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		if($this->operandSize == "variable") {
			if($this->order == "row-major") {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "$cType *__restrict buffer = do_alloca(MATRIX1_ROWS * MATRIX2_COLS * sizeof($cType), use_heap);";
				$lines[] = "uint32_t i, j, k, p, q, res_index = 0;";
				$lines[] = "for(i = 0, q = 0; i < MATRIX1_ROWS; ++i) {";
				$lines[] = 		"for(j = 0; j < MATRIX2_COLS; ++j) {";
				$lines[] = 			"$cType dot_product = 0;";
				$lines[] = 			"for(p = 0, k = j; p < MATRIX2_ROWS; ++p, k += MATRIX2_COLS) {";
				$lines[] = 				"dot_product += op1_ptr[p + q] * op2_ptr[k];";
				$lines[] = 			"}";
				$lines[] = 			"buffer[res_index++] = dot_product;";
				$lines[] = 		"}";
				$lines[] = 		"q += MATRIX2_ROWS;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, MATRIX1_ROWS * MATRIX2_COLS * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			} else {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "$cType *__restrict buffer = do_alloca(MATRIX1_ROWS * MATRIX2_COLS * sizeof($cType), use_heap);";
				$lines[] = "uint32_t i, j, k, p, q, res_index = 0;";
				$lines[] = "for(i = 0, q = 0; i < MATRIX2_COLS; ++i) {";
				$lines[] = 		"for(j = 0; j < MATRIX1_ROWS; ++j) {";
				$lines[] = 			"$cType dot_product = 0;";
				$lines[] = 			"for(p = 0, k = 0; p < MATRIX1_COLS; ++p, k += MATRIX1_ROWS) {";
				$lines[] = 				"dot_product += op1_ptr[k + j] * op2_ptr[p + q];";
				$lines[] = 			"}";
				$lines[] = 			"buffer[res_index++] = dot_product;";
				$lines[] = 		"}";
				$lines[] = 		"q += MATRIX1_COLS;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, MATRIX1_ROWS * MATRIX2_COLS * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			}
		} else {
			if($this->order == "row-major") {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0 * 2 + 1] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[0 * 2 + 1]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "res_ptr[0 * 2 + 0] = dot_product0;";
						$lines[] = "res_ptr[1 * 2 + 0] = dot_product1;";
						$lines[] = "res_ptr[0 * 2 + 1] = dot_product2;";
						$lines[] = "res_ptr[1 * 2 + 1] = dot_product3;";
					}	break;
					case 3: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[0 * 3 + 2]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 3 + 0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[1 * 3 + 2]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0 * 3 + 0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2 * 3 + 2]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0 * 3 + 1] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[0 * 3 + 2]);";
						$lines[] = "$cType dot_product4 = (op1_ptr[0 * 3 + 1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1 * 3 + 2]);";
						$lines[] = "$cType dot_product5 = (op1_ptr[0 * 3 + 1] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2 * 3 + 2]);";
						$lines[] = "$cType dot_product6 = (op1_ptr[0 * 3 + 2] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[0 * 3 + 2]);";
						$lines[] = "$cType dot_product7 = (op1_ptr[0 * 3 + 2] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[1 * 3 + 2]);";
						$lines[] = "$cType dot_product8 = (op1_ptr[0 * 3 + 2] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2 * 3 + 2]);";
						$lines[] = "res_ptr[0 * 3 + 0] = dot_product0;";
						$lines[] = "res_ptr[1 * 3 + 0] = dot_product1;";
						$lines[] = "res_ptr[2 * 3 + 0] = dot_product2;";
						$lines[] = "res_ptr[0 * 3 + 1] = dot_product3;";
						$lines[] = "res_ptr[1 * 3 + 1] = dot_product4;";
						$lines[] = "res_ptr[2 * 3 + 1] = dot_product5;";
						$lines[] = "res_ptr[0 * 3 + 2] = dot_product6;";
						$lines[] = "res_ptr[1 * 3 + 2] = dot_product7;";
						$lines[] = "res_ptr[2 * 3 + 2] = dot_product8;";
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 2]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[1 * 4 + 2]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2 * 4 + 2]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0 * 4 + 1] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[0 * 4 + 2]);";
						$lines[] = "$cType dot_product4 = (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 2]);";
						$lines[] = "$cType dot_product5 = (op1_ptr[0 * 4 + 1] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2 * 4 + 2]);";
						$lines[] = "$cType dot_product6 = (op1_ptr[0 * 4 + 2] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[0 * 4 + 2]);";
						$lines[] = "$cType dot_product7 = (op1_ptr[0 * 4 + 2] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[1 * 4 + 2]);";
						$lines[] = "$cType dot_product8 = (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 2]);";
						$lines[] = "res_ptr[0 * 4 + 0] = dot_product0;";
						$lines[] = "res_ptr[1 * 4 + 0] = dot_product1;";
						$lines[] = "res_ptr[2 * 4 + 0] = dot_product2;";
						$lines[] = "res_ptr[0 * 4 + 1] = dot_product3;";
						$lines[] = "res_ptr[1 * 4 + 1] = dot_product4;";
						$lines[] = "res_ptr[2 * 4 + 1] = dot_product5;";
						$lines[] = "res_ptr[0 * 4 + 2] = dot_product6;";
						$lines[] = "res_ptr[1 * 4 + 2] = dot_product7;";
						$lines[] = "res_ptr[2 * 4 + 2] = dot_product8;";
					}	break;
				}
			} else {
				switch($this->operandSize) {
					case 2: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 0]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);";
						$lines[] = "res_ptr[0 * 2 + 0] = dot_product0;";
						$lines[] = "res_ptr[0 * 2 + 1] = dot_product1;";
						$lines[] = "res_ptr[1 * 2 + 0] = dot_product2;";
						$lines[] = "res_ptr[1 * 2 + 1] = dot_product3;";
					}	break;
					case 3: {
						if($this->operandPadding) {
							$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 0]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 1]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 2]);";
							$lines[] = "$cType dot_product3 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 0]);";
							$lines[] = "$cType dot_product4 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 1]);";
							$lines[] = "$cType dot_product5 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 2]);";
							$lines[] = "$cType dot_product6 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 0]);";
							$lines[] = "$cType dot_product7 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 1]);";
							$lines[] = "$cType dot_product8 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 2]);";
							$lines[] = "res_ptr[0 * 4 + 0] = dot_product0;";
							$lines[] = "res_ptr[0 * 4 + 1] = dot_product1;";
							$lines[] = "res_ptr[0 * 4 + 2] = dot_product2;";
							$lines[] = "res_ptr[1 * 4 + 0] = dot_product3;";
							$lines[] = "res_ptr[1 * 4 + 1] = dot_product4;";
							$lines[] = "res_ptr[1 * 4 + 2] = dot_product5;";
							$lines[] = "res_ptr[2 * 4 + 0] = dot_product6;";
							$lines[] = "res_ptr[2 * 4 + 1] = dot_product7;";
							$lines[] = "res_ptr[2 * 4 + 2] = dot_product8;";
						} else {
							$lines[] = "$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[0 * 3 + 2] * op2_ptr[2 * 3 + 0]);";
							$lines[] = "$cType dot_product1 = (op1_ptr[0 * 3 + 0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[0 * 3 + 2] * op2_ptr[2 * 3 + 1]);";
							$lines[] = "$cType dot_product2 = (op1_ptr[0 * 3 + 0] * op2_ptr[0 * 3 + 2]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1 * 3 + 2]) + (op1_ptr[0 * 3 + 2] * op2_ptr[2 * 3 + 2]);";
							$lines[] = "$cType dot_product3 = (op1_ptr[1 * 3 + 0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2 * 3 + 0]);";
							$lines[] = "$cType dot_product4 = (op1_ptr[1 * 3 + 0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2 * 3 + 1]);";
							$lines[] = "$cType dot_product5 = (op1_ptr[1 * 3 + 0] * op2_ptr[0 * 3 + 2]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1 * 3 + 2]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2 * 3 + 2]);";
							$lines[] = "$cType dot_product6 = (op1_ptr[2 * 3 + 0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2 * 3 + 0]);";
							$lines[] = "$cType dot_product7 = (op1_ptr[2 * 3 + 0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2 * 3 + 1]);";
							$lines[] = "$cType dot_product8 = (op1_ptr[2 * 3 + 0] * op2_ptr[0 * 3 + 2]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1 * 3 + 2]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2 * 3 + 2]);";
							$lines[] = "res_ptr[0 * 3 + 0] = dot_product0;";
							$lines[] = "res_ptr[0 * 3 + 1] = dot_product1;";
							$lines[] = "res_ptr[0 * 3 + 2] = dot_product2;";
							$lines[] = "res_ptr[1 * 3 + 0] = dot_product3;";
							$lines[] = "res_ptr[1 * 3 + 1] = dot_product4;";
							$lines[] = "res_ptr[1 * 3 + 2] = dot_product5;";
							$lines[] = "res_ptr[2 * 3 + 0] = dot_product6;";
							$lines[] = "res_ptr[2 * 3 + 1] = dot_product7;";
							$lines[] = "res_ptr[2 * 3 + 2] = dot_product8;";
						}
					}	break;
					case 4: {
						$lines[] = "$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op2_ptr[3 * 4 + 0]);";
						$lines[] = "$cType dot_product1 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[0 * 4 + 3] * op2_ptr[3 * 4 + 1]);";
						$lines[] = "$cType dot_product2 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 3] * op2_ptr[3 * 4 + 2]);";
						$lines[] = "$cType dot_product3 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 3]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 3]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 3]) + (op1_ptr[0 * 4 + 3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "$cType dot_product4 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3 * 4 + 0]);";
						$lines[] = "$cType dot_product5 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3 * 4 + 1]);";
						$lines[] = "$cType dot_product6 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3 * 4 + 2]);";
						$lines[] = "$cType dot_product7 = (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 3]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 3]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 3]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "$cType dot_product8 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3 * 4 + 0]);";
						$lines[] = "$cType dot_product9 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3 * 4 + 1]);";
						$lines[] = "$cType dot_product10 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3 * 4 + 2]);";
						$lines[] = "$cType dot_product11 = (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 3]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 3]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 3]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "$cType dot_product12 = (op1_ptr[3 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3 * 4 + 0]);";
						$lines[] = "$cType dot_product13 = (op1_ptr[3 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3 * 4 + 1]);";
						$lines[] = "$cType dot_product14 = (op1_ptr[3 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3 * 4 + 2]);";
						$lines[] = "$cType dot_product15 = (op1_ptr[3 * 4 + 0] * op2_ptr[0 * 4 + 3]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1 * 4 + 3]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2 * 4 + 3]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3 * 4 + 3]);";
						$lines[] = "res_ptr[0 * 4 + 0] = dot_product0;";
						$lines[] = "res_ptr[0 * 4 + 1] = dot_product1;";
						$lines[] = "res_ptr[0 * 4 + 2] = dot_product2;";
						$lines[] = "res_ptr[0 * 4 + 3] = dot_product3;";
						$lines[] = "res_ptr[1 * 4 + 0] = dot_product4;";
						$lines[] = "res_ptr[1 * 4 + 1] = dot_product5;";
						$lines[] = "res_ptr[1 * 4 + 2] = dot_product6;";
						$lines[] = "res_ptr[1 * 4 + 3] = dot_product7;";
						$lines[] = "res_ptr[2 * 4 + 0] = dot_product8;";
						$lines[] = "res_ptr[2 * 4 + 1] = dot_product9;";
						$lines[] = "res_ptr[2 * 4 + 2] = dot_product10;";
						$lines[] = "res_ptr[2 * 4 + 3] = dot_product11;";
						$lines[] = "res_ptr[3 * 4 + 0] = dot_product12;";
						$lines[] = "res_ptr[3 * 4 + 1] = dot_product13;";
						$lines[] = "res_ptr[3 * 4 + 2] = dot_product14;";
						$lines[] = "res_ptr[3 * 4 + 3] = dot_product15;";
					}	break;
				}
			}
		}
		return $lines;
	}
}

?>