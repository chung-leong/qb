<?php

class QBMultiplyMatrixByVectorHandler extends QBHandler {

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2:	return "U32";	// number of rows in M
			case 3:	return "U32";	// number of columns in M
			case 4: return $this->operandType;
			case 5: return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "VAR";
			case 3: return "VAR";
			case 4: return "ARR";
			case 5: return "ARR";
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_$type(qb_interpreter_context *cxt, $cType *m, uint32_t m_row, uint32_t m_col, $cType *v, uint32_t v_row, $cType *res_ptr) {",
					"uint32_t i, j, k;",
					"for(i = 0, k = 0; i < m_row; ++i) {",
						"$cType dot_product = 0;",
						"for(j = 0; j < m_col; ++j) {",
							"dot_product += m[k++] * v[j];",
						"}",
						"res_ptr[i] = dot_product;",
					"}",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return "op4_count";
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "if(op3 != op4_count) {";
		$lines[] = 		"*cxt->line_number_pointer = PHP_LINE_NUMBER;";
		$lines[] = 		"qb_abort(\"Number of columns in matrix (%d) does not match size of vector (%d)\", op3, op4_count);";
		$lines[] = "}";
		$lines[] = "qb_multiply_matrix_by_vector_$type(cxt, op1_ptr, op2, op3, op4_ptr, op4_count, res_ptr);";
		return $lines;
	}
}

?>