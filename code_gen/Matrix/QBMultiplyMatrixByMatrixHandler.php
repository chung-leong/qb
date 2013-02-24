<?php

class QBMultiplyMatrixByMatrixHandler extends QBHandler {

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2:	return "U32";	// number of rows in M1
			case 3:	return "U32";	// number of columns in M1
			case 4: return $this->operandType;
			case 5:	return "U32";	// number of rows in M2
			case 6:	return "U32";	// number of columns in M2
			case 7: return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "VAR";
			case 3: return "VAR";
			case 4: return "ARR";
			case 5: return "VAR";
			case 6: return "VAR";
			case 7: return "ARR";
		}
	}


	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_$type(qb_interpreter_context *cxt, $cType *m1, uint32_t m1_row, uint32_t m1_col, $cType *m2, uint32_t m2_row, uint32_t m2_col, $cType *res_ptr) {",
					"uint32_t i, j, k, p, q, res_index = 0;",
					"for(i = 0, q = 0; i < m1_row; ++i) {",
						"for(j = 0; j < m2_col; ++j) {",
							"$cType dot_product = 0;",
							"for(p = 0, k = 0; p < m2_row; ++p, k += m2_col) {",
								"dot_product += m1[p + q] * m2[k + j];",
							"}",
							"res_ptr[res_index++] = dot_product;",
						"}",
						"q += m1_col;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		return 2;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "if(op3 != op5) {";
		$lines[] = 		"*cxt->line_number_pointer = PHP_LINE_NUMBER;";
		$lines[] = 		"qb_abort(\"Number of columns in first matrix (%d) does not match number of rows in second matrix (%d)\", op3, op5);";
		$lines[] = "}";
		$lines[] = "qb_multiply_matrix_by_matrix_$type(cxt, op1_ptr, op2, op3, op4_ptr, op5, op6, res_ptr);";
		return $lines;
	}
}

?>