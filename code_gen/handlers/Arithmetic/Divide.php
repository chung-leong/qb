<?php

class Divide extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication, Multithreaded;
	
	public function needsInterpreterContext() {
		$type = $this->getOperandType(1);
		if($type[0] != 'F') {
			return true;
		}
	}
	
	public function needsLineIdentifier($where = null) {
		$type = $this->getOperandType(1);
		if($type[0] != 'F') {
			return true;
		} else {
			return parent::needsLineIdentifier($where);
		}
	}
	
	public function mayExitLoop() {
		$type = $this->getOperandType(1);
		if($type[0] != 'F') {
			return true;
		}
	}
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$lines = array();
		if($type[0] != 'F') {
			$lines[] = "if(UNEXPECTED(op2 == 0)) {";
			$lines[] = 		"qb_report_divide_by_zero_exception(line_id);";
			$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
			$lines[] =		"return;";
			$lines[] = "}";
		}
		$lines[] = "res = op1 / op2;";
		return $lines;
	}
}

?>
