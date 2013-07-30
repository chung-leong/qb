<?php

class QBDivideHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		$lines = array();
		if($type[0] != 'F') {
			$lines[] = "if(UNEXPECTED(op2 == 0)) {";
			$lines[] = 		"qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);";
			$lines[] = "}";
		}
		$lines[] = "res = op1 / op2;";
		return $lines;
	}
}

?>