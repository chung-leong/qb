<?php

class QBMultiplyVectorByMatrixHandler extends QBHandler {

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3:	return "U32";	// number of rows in M
			case 4:	return "U32";	// number of columns in M
			case 5: return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "VAR";
			case 4: return "VAR";
			case 5: return "ARR";
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_$type(qb_interpreter_context *cxt, $cType *v, uint32_t v_col, $cType *m, uint32_t m_row, uint32_t m_col, $cType *res_ptr) {",
					"uint32_t i, j, k;",
					"for(i = 0; i < m_col; ++i) {",
						"$cType dot_product = 0;",
						"for(j = 0, k = 0; j < m_row; ++j) {",
							"dot_product += v[j] * m[k + i];",
							"k += m_col;",
						"}",
						"res_ptr[i] = dot_product;",
					"}",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return "op4";
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "if(op3 != op1_count) {";
		$lines[] = 		"*cxt->line_number_pointer = PHP_LINE_NUMBER;";
		$lines[] = 		"qb_abort(\"Number of rows in matrix (%d) does not match size of vector (%d)\", op3, op1_count);";
		$lines[] = "}";
		$lines[] = "qb_multiply_vector_by_matrix_$type(cxt, op1_ptr, op1_count, op2_ptr, op3, op4, res_ptr);";
		return $lines;
	}
}

?>