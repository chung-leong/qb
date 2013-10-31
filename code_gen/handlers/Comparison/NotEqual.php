<?php

class NotEqual extends Handler {

	use MultipleAddressMode, BinaryOperator, BooleanResult, ScalarResult, ArrayComparison;
	
	protected function getActionOnUnitData() {
		return "res = (op1 != op2);";
	}

	protected function getActionOnMultipleData() {
		$type = $this->getOperandType(1);
		$type = preg_replace('/^I/', 'S', $type);
		return "res = (qb_compare_array_$type(op1_ptr, op1_count, op2_ptr, op2_count) != 0);";
	}	
}

?>