<?php

class QBDistanceHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1:
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$sqrt = ($type == "F64") ? 'sqrt' : 'sqrtf';
		$functions = array(
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_$type(qb_interpreter_context *__restrict cxt, $cType *v1, $cType *v2, uint32_t count) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < count; i++) {",
						"sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);",
					"}",
					"return $sqrt(sum);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "if(op1_count != op2_count) {";
		$lines[] = 		"*cxt->line_number_pointer = PHP_LINE_NUMBER;";
		$lines[] = 		"qb_abort(\"Size of first array (%d) does not match size of second array (%d)\", op1_count, op2_count);";
		$lines[] = "}";
		$lines[] = "res = qb_calculate_array_distance_$type(cxt, op1_ptr, op2_ptr, op1_count);";
		return $lines;
	}

}

?>