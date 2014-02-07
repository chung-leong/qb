<?php

// note: not affected by matrix order since determinant of M is the same as the determinant of the transpose of M

class Determinant extends Handler {

	use MultipleAddressMode, UnaryOperator, UnitResult, FloatingPointOnly, Multithreaded;
	
	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 2;
		} else {
			return 1;
		}
	}
	
	public function getOperandAddressMode($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "ARR";
				case 2: return "SCA";
				case 3: return "ARR";
			}
		} else {
			switch($i) {
				case 1: return "ARR";
				case 2: return $this->addressMode;
			}
		}
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op2 * op2";
				case 2: return 1;
				case 3: return 1;
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize * $this->operandSize;
				case 2: return 1;
			}
			
		}
	}
	
	public function getOperandType($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return "U32";
				case 3: return $this->operandType;
			}
		} else {
			switch($i) {
				case 1: return $this->operandType;
				case 2: return $this->operandType;
			}
		}
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		if($this->operandSize == "variable") {
			$determinantHandler4X = new Determinant(NULL, $this->operandType, "SCA", 4);
			$determinantHandler = new Determinant(NULL, $this->operandType, "SCA", "variable");
			$determinantFunction4X = $determinantHandler4X->getHandlerFunctionName();
			$determinantFunction = $determinantHandler->getHandlerFunctionName();
			
			$lines[] = "uint32_t matrix_rows = op2, matrix_cols = op2;";
			$lines[] = "if(matrix_rows == 4) {";
			$lines[] = 		"$determinantFunction4X(op1_ptr, res_ptr);";
			$lines[] = "} else {";
			$lines[] = 		"ALLOCA_FLAG(use_heap)";
			$lines[] =		"uint32_t minor_size = (matrix_rows - 1) * (matrix_cols - 1);";
			$lines[] = 		"$cType *__restrict minor = do_alloca(minor_size * sizeof($cType), use_heap);";
			$lines[] = 		"uint32_t i, j, k, m, n;";
			$lines[] = 		"$cType sign = 1, det = 0;";
			$lines[] = 		"for(m = 0; m < matrix_rows; m++) {";
			$lines[] = 			"$cType a = op1_ptr[m];";
			$lines[] = 			"$cType minor_det;";
			$lines[] = 			"for(i = 1, n = 0, k = matrix_rows; i < matrix_rows; i++) {";
			$lines[] = 				"for(j = 0; j < matrix_rows; j++, k++) {";
			$lines[] = 					"if(j != m) {";
			$lines[] = 						"minor[n] = op1_ptr[k];";
			$lines[] = 						"n++;";
			$lines[] = 					"}";
			$lines[] = 				"}";
			$lines[] = 			"}";
			$lines[] = 			"$determinantFunction(minor, 0, matrix_rows - 1, &minor_det);";
			$lines[] = 			"det += a * minor_det * sign;";
			$lines[] = 			"sign = -sign;";
			$lines[] = 		"}";
			$lines[] =		"free_alloca(minor, use_heap);";
			$lines[] = 		"res = det;";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "res = (op1_ptr[0 * 2 + 0] * op1_ptr[1 * 2 + 1]) - (op1_ptr[0 * 2 + 1] * op1_ptr[1 * 2 + 0]);";
				}	break;
				case 3: {
					$lines[] = "res =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -";
					$lines[] = 			"(op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]) +";
					$lines[] = 			"(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) -";
					$lines[] = 			"(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]) +";
					$lines[] = 			"(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) -";
					$lines[] =			"(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);";
				}	break;
				case 4: {
					$lines[] = "res = 	 ((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) +";
					$lines[] = 			"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) +";
					$lines[] = 			"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) +";
					$lines[] = 			"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) +";
					$lines[] = 			"((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) +";
					$lines[] = 			"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) -";
					$lines[] = 			"((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]));";
				}	break;
			}
		}
		return $lines;
	}
}

?>