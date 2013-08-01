<?php

class QBEqualHandler extends QBCompareHandler {

	protected function getActionForMultipleData() {
		$type = $this->getOperandType(1);
		$type = preg_replace('/^I/', 'S', $type);
		return "res = (qb_compare_array_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count) == 0);";
	}
	
	protected function getActionForUnitData() {
		return "res = (op1 == op2);";
	}
}

?>