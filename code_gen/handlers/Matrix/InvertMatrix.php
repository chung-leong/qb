<?php

// note: not affected by matrix order since inverse of M is the same as the inverse of the transpose of M

class InvertMatrix extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getInputOperandCount() {
		if($this->operandSize == "variable") {
			return 2;
		} else {
			return parent::getInputOperandCount();
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
			return parent::getOperandAddressMode($i);
		}
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "op2 * op2";
				case 2: return 1;
				case 3: return "op2 * op2";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize * $this->operandSize;
				case 2: return $this->operandSize * $this->operandSize;
			}
			
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == "F32") ? 'f' : '';
		$lines = array();
		if($this->operandSize == "variable") {
			$determinantHandler = new Determinant(NULL, $this->operandType, "SCA", "variable");
			$determinantFunction = $determinantHandler->getHandlerFunctionName();
			
			$lines[] = "ALLOCA_FLAG(use_heap1)";
			$lines[] = "ALLOCA_FLAG(use_heap2)";
			$lines[] = "uint32_t i, j, k, m, n, p, q;";
			$lines[] = "uint32_t matrix_rows = op2, matrix_cols = op2;";
			$lines[] = "$cType *__restrict minor = do_alloca((matrix_rows - 1) * (matrix_cols - 1) * sizeof($cType), use_heap1);";
			$lines[] = "$cType *__restrict cofactors = do_alloca(matrix_rows * matrix_cols * sizeof($cType), use_heap2);";
			$lines[] = "$cType a, sign_init = 1, sign, det = 0, rdet;";
			$lines[] = "for(m = 0, p = 0; m < matrix_rows; m++) {";
			$lines[] = 		"sign = sign_init;";
			$lines[] = 		"sign_init = -sign_init;";
			$lines[] = 		"for(n = 0; n < matrix_rows; n++) {";
			$lines[] = 			"$cType minor_det;";
			$lines[] = 			"for(i = 0, k = 0, q = 0; i < matrix_rows; i++) {";
			$lines[] = 				"for(j = 0; j < matrix_rows; j++, k++) {";
			$lines[] = 					"if(j != m && i != n) {";
			$lines[] = 						"minor[q] = op1_ptr[k];";
			$lines[] = 						"q++;";
			$lines[] = 					"}";
			$lines[] = 				"}";
			$lines[] = 			"}";
			$lines[] = 			"$determinantFunction(minor, matrix_rows - 1, matrix_cols - 1, &minor_det);";
			$lines[] = 			"cofactors[p] = minor_det * sign;";
			$lines[] = 			"p++;";
			$lines[] = 			"if(n == 0) {";
			$lines[] = 				"a = op1_ptr[m];";
			$lines[] = 				"det += a * minor_det * sign;";
			$lines[] = 			"}";
			$lines[] = 			"sign = -sign;";
			$lines[] = 		"}";
			$lines[] = "}";
			$lines[] = "if(det != 0) {";
			$lines[] = 		"rdet = 1 / det;";
			$lines[] = 		"for(p = 0; p < matrix_rows * matrix_cols; p++) {";
			$lines[] = 			"res_ptr[p] = cofactors[p] * rdet;";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"for(p = 0; p < matrix_rows * matrix_cols; p++) {";
			$lines[] = 			"res_ptr[p] = ($cType) NAN;";
			$lines[] = 		"}";
			$lines[] = "}";
			$lines[] = "free_alloca(minor, use_heap1);";
			$lines[] = "free_alloca(cofactors, use_heap2);";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType p00 =  op1_ptr[1 * 2 + 1];";
					$lines[] = "$cType p01 = -op1_ptr[0 * 2 + 1];";
					$lines[] = "$cType p10 = -op1_ptr[1 * 2 + 0];";
					$lines[] = "$cType p11 =  op1_ptr[0 * 2 + 0];";
					$lines[] = "$cType det = (op1_ptr[0 * 2 + 0] * p00) + (op1_ptr[0 * 2 + 1] * p10);";
					$lines[] = "if(det != 0) {";
					$lines[] = 		"$cType rdet = 1.0$f / det;";
					$lines[] = 		"res_ptr[0 * 2 + 0] = p00 * rdet;";
					$lines[] = 		"res_ptr[0 * 2 + 1] = p01 * rdet;";
					$lines[] = 		"res_ptr[1 * 2 + 0] = p10 * rdet;";
					$lines[] = 		"res_ptr[1 * 2 + 1] = p11 * rdet;";
					$lines[] = "} else {";
					$lines[] = 		"uint32_t i;";
					$lines[] = 		"for(i = 0; i < 4; i++) {";
					$lines[] = 			"res_ptr[i] = ($cType) NAN;";
					$lines[] = 		"}";
					$lines[] = "}";
				}	break;
				case 3: {
					$lines[] = "$cType p00 = (op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) - (op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]);";
					$lines[] = "$cType p01 = (op1_ptr[0 * 3 + 2] * op1_ptr[2 * 3 + 1]) - (op1_ptr[0 * 3 + 1] * op1_ptr[2 * 3 + 2]);";
					$lines[] = "$cType p02 = (op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2]) - (op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1]);";
					$lines[] = "$cType p10 = (op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) - (op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]);";
					$lines[] = "$cType p11 = (op1_ptr[0 * 3 + 0] * op1_ptr[2 * 3 + 2]) - (op1_ptr[0 * 3 + 2] * op1_ptr[2 * 3 + 0]);";
					$lines[] = "$cType p12 = (op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0]) - (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2]);";
					$lines[] = "$cType p20 = (op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) - (op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);";
					$lines[] = "$cType p21 = (op1_ptr[0 * 3 + 1] * op1_ptr[2 * 3 + 0]) - (op1_ptr[0 * 3 + 0] * op1_ptr[2 * 3 + 1]);";
					$lines[] = "$cType p22 = (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1]) - (op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0]);";
					$lines[] = "$cType det = (op1_ptr[0 * 3 + 0] * p00) + (op1_ptr[0 * 3 + 1] * p10) + (op1_ptr[0 * 3 + 2] * p20);";
					$lines[] = "if(det != 0) {";
					$lines[] = 		"$cType rdet = 1.0$f / det;";
					$lines[] = 		"res_ptr[0 * 3 + 0] = p00 * rdet;";
					$lines[] = 		"res_ptr[0 * 3 + 1] = p01 * rdet;";
					$lines[] = 		"res_ptr[0 * 3 + 2] = p02 * rdet;";
					$lines[] = 		"res_ptr[1 * 3 + 0] = p10 * rdet;";
					$lines[] = 		"res_ptr[1 * 3 + 1] = p11 * rdet;";
					$lines[] = 		"res_ptr[1 * 3 + 2] = p12 * rdet;";
					$lines[] = 		"res_ptr[2 * 3 + 0] = p20 * rdet;";
					$lines[] = 		"res_ptr[2 * 3 + 1] = p21 * rdet;";
					$lines[] = 		"res_ptr[2 * 3 + 2] = p22 * rdet;";
					$lines[] = "} else {";
					$lines[] = 		"uint32_t i;";
					$lines[] = 		"for(i = 0; i < 9; i++) {";
					$lines[] = 			"res_ptr[i] = ($cType) NAN;";
					$lines[] = 		"}";
					$lines[] = "}";
				}	break;
				case 4: {
					$lines[] = "$cType p00 = (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]) + (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p01 = (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p02 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p03 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3]);";
					$lines[] = "$cType p10 = (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p11 = (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p12 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p13 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3]);";
					$lines[] = "$cType p20 = (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p21 = (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p22 = (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 3]);";
					$lines[] = "$cType p23 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3]);";
					$lines[] = "$cType p30 = (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]);";
					$lines[] = "$cType p31 = (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]);";
					$lines[] = "$cType p32 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 2]);";
					$lines[] = "$cType p33 = (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2]);";
					$lines[] = "$cType det = (op1_ptr[0 * 4 + 0] * p00) + (op1_ptr[0 * 4 + 1] * p10) + (op1_ptr[0 * 4 + 2] * p20) + (op1_ptr[0 * 4 + 3] * p30);";
					$lines[] = "if(det != 0) {";
					$lines[] = 		"$cType rdet = 1.0$f / det;";
					$lines[] = 		"res_ptr[0 * 4 + 0] = p00 * rdet;";
					$lines[] = 		"res_ptr[0 * 4 + 1] = p01 * rdet;";
					$lines[] = 		"res_ptr[0 * 4 + 2] = p02 * rdet;";
					$lines[] = 		"res_ptr[0 * 4 + 3] = p03 * rdet;";
					$lines[] = 		"res_ptr[1 * 4 + 0] = p10 * rdet;";
					$lines[] = 		"res_ptr[1 * 4 + 1] = p11 * rdet;";
					$lines[] = 		"res_ptr[1 * 4 + 2] = p12 * rdet;";
					$lines[] = 		"res_ptr[1 * 4 + 3] = p13 * rdet;";
					$lines[] = 		"res_ptr[2 * 4 + 0] = p20 * rdet;";
					$lines[] = 		"res_ptr[2 * 4 + 1] = p21 * rdet;";
					$lines[] = 		"res_ptr[2 * 4 + 2] = p22 * rdet;";
					$lines[] = 		"res_ptr[2 * 4 + 3] = p23 * rdet;";
					$lines[] = 		"res_ptr[3 * 4 + 0] = p30 * rdet;";
					$lines[] = 		"res_ptr[3 * 4 + 1] = p31 * rdet;";
					$lines[] = 		"res_ptr[3 * 4 + 2] = p32 * rdet;";
					$lines[] = 		"res_ptr[3 * 4 + 3] = p33 * rdet;";
					$lines[] = "} else {";
					$lines[] = 		"uint32_t i;";
					$lines[] = 		"for(i = 0; i < 16; i++) {";
					$lines[] = 			"res_ptr[i] = ($cType) NAN;";
					$lines[] = 		"}";
					$lines[] = "}";
				}	break;
			}
		}
		return $lines;
	}
}

?>