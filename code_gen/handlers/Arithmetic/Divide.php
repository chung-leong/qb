<?php

class Divide extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication;
	
	public function needsInterpreterContext() {
		$type = $this->getOperandType(1);
		if($type[0] != 'F') {
			return true;
		}
	}
	
	public function needsLineNumber($where = null) {
		$type = $this->getOperandType(1);
		if($type[0] != 'F') {
			return true;
		} else {
			return parent::needsLineNumber($where);
		}
	}
	
	public function getHelperFunctions() {
		$functions = array(
			array(
				"NO_RETURN void qb_abort_divide_by_zero_error(qb_interpreter_context *restrict cxt, uint32_t line_number) {",
					"USE_TSRM",
					"QB_G(current_filename) = cxt->function->filename;",
					"QB_G(current_line_number) = line_number;",
					"qb_abort(\"Division by zero\");",
				"}",
			),
		);
		return $functions;
	}

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$lines = array();
		if($type[0] != 'F') {
			$lines[] = "if(UNEXPECTED(op2 == 0)) {";
			$lines[] = 		"qb_abort_divide_by_zero_error(cxt, line_number);";
			$lines[] = "}";
		}
		$lines[] = "res = op1 / op2;";
		return $lines;
	}
}

?>