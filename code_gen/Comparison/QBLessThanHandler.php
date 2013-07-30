<?php

class QBLessThanHandler extends QBCompareHandler {

	protected function getArrayExpression() {
		$type = $this->getOperandType(1);
		return "res = (qb_compare_array_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count) == -1);";
	}
	
	protected function getScalarExpression() {
		return "res = (op1 < op2);";
	}
}

?>