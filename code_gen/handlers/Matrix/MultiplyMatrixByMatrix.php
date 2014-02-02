<?php

class MultiplyMatrixByMatrix extends Handler {

	use ArrayAddressMode, BinaryOperator, MatrixConventionDependent, FloatingPointOnly, Multithreaded;
	
	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 5;
		} else {
			return 2;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "ARR";
				case 3: return "SCA";		//	m1 rows
				case 4: return "SCA";		//  m1 cols / m2 rows
				case 5: return "SCA";		//  m2 cols
				case 6: return "ARR";
			}
		} else {
			return parent::getOperandAddressMode($i);
		}
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "(op3 * op4)";
				case 2: return "(op4 * op5)";
				case 3: return 1;
				case 4: return 1;
				case 5: return 1;
				case 6: return "(op3 * op5)";
			}
		} else {
			$padding = $this->getPadding();
			$matrixSize = ($this->operandSize + $padding) * $this->operandSize;
			switch($i) {
				case 1: return $matrixSize;
				case 2: return $matrixSize;
				case 3: return $matrixSize;
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
				case 5: return "U32";
				case 6: return $this->operandType;
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
				$lines[] = "uint32_t i, j, k, p, q, res_index = 0;";
				$lines[] = "uint32_t matrix1_rows = op3, matrix2_rows = op4, matrix2_cols = op5;";
				$lines[] = "$cType *__restrict buffer = do_alloca(matrix1_rows * matrix2_cols * sizeof($cType), use_heap);";
				$lines[] = "for(i = 0, q = 0; i < matrix1_rows; ++i) {";
				$lines[] = 		"for(j = 0; j < matrix2_cols; ++j) {";
				$lines[] = 			"$cType dot_product = 0;";
				$lines[] = 			"for(p = 0, k = j; p < matrix2_rows; ++p, k += matrix2_cols) {";
				$lines[] = 				"dot_product += op1_ptr[p + q] * op2_ptr[k];";
				$lines[] = 			"}";
				$lines[] = 			"buffer[res_index++] = dot_product;";
				$lines[] = 		"}";
				$lines[] = 		"q += matrix2_rows;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, matrix1_rows * matrix2_cols * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			} else {
				$lines[] = "ALLOCA_FLAG(use_heap)";
				$lines[] = "uint32_t i, j, k, p, q, res_index = 0;";
				$lines[] = "uint32_t matrix1_rows = op3, matrix1_cols = op4, matrix2_cols = op5;";
				$lines[] = "$cType *__restrict buffer = do_alloca(matrix1_rows * matrix2_cols * sizeof($cType), use_heap);";
				$lines[] = "for(i = 0, q = 0; i < matrix2_cols; ++i) {";
				$lines[] = 		"for(j = 0; j < matrix1_rows; ++j) {";
				$lines[] = 			"$cType dot_product = 0;";
				$lines[] = 			"for(p = 0, k = 0; p < matrix1_cols; ++p, k += matrix1_rows) {";
				$lines[] = 				"dot_product += op1_ptr[k + j] * op2_ptr[p + q];";
				$lines[] = 			"}";
				$lines[] = 			"buffer[res_index++] = dot_product;";
				$lines[] = 		"}";
				$lines[] = 		"q += matrix1_cols;";
				$lines[] = "}";
				$lines[] = "memcpy(res_ptr, buffer, matrix1_rows * matrix2_cols * sizeof($cType));";
				$lines[] = "free_alloca(buffer, use_heap);";
			}
		} else {
			if($order == "row-major") {
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
						if($this->getPadding()) {
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