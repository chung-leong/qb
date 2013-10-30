<?php

class ArrayColumn extends Handler {

	use ArrayAddressMode, QuaternaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U32";
			case 4: return "U32";
			case 5: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "SCA";
			case 5: return "ARR";
		}
	}
		
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "op2_count";
			case 3: return 1;
			case 4: return 1;
			case 5: return "res_count";
		}
	}
	
	public function getHelperFunctions() {
		$functions = array(
			array(
				"NO_RETURN void qb_abort_missing_column(qb_interpreter_context *restrict cxt, uint32_t column_offset, uint32_t line_number) {",
					"USE_TSRM",
					"QB_G(current_line_number) = line_number;",
					"qb_abort(\"Accessing non-existing column at index %d\", column_offset);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "uint32_t column_offset = op4, column_count = op2, element_size = op3;";
		$lines[] = "if(UNEXPECTED(column_offset >= column_count)) {";
		$lines[] =		"qb_abort_missing_column(cxt, column_offset, line_number);";
		$lines[] = "}";
		$lines[] = "op1_ptr += column_offset * element_size;";
		$lines[] = "if(element_size == 1) {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] =			"*res_ptr = *op1_ptr;";
		$lines[] =			"res_ptr += 1;";
		$lines[] =			"op1_ptr += column_count * element_size;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] =			"memcpy(res_ptr, op1_ptr, element_size * sizeof($cType));";
		$lines[] =			"res_ptr += element_size;";
		$lines[] =			"op1_ptr += column_count * element_size;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>