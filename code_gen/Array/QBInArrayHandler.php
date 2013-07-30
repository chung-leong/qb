<?php

class QBInArrayHandler extends QBArraySearchHandler {
	
	public function getInputOperandCount() {
		return 2;
	}

	protected function getArrayExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_find_elements_$type(op1_ptr, op1_count, op2_ptr, op2_count) != -1;";
	}
	
	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_find_element_$type(op1_ptr, op1_count, op2) != -1;";
	}
}

?>