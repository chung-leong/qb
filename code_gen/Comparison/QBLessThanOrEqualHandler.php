<?php

class QBLessThanOrEqualHandler extends QBCompareHandler {

	protected function getActionOnUnitData() {
		return "res = (op1 <= op2);";
	}
	
	protected function getActionOnMultipleData() {
		$type = $this->getOperandType(1);
		return "res = (qb_compare_array_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count) != 1);";
	}	 
}

?>