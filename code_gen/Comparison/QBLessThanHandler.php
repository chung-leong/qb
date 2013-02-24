<?php

class QBLessThanHandler extends QBCompareHandler {

	protected function getArrayExpression() {
		$type = $this->getOperandType(1);
		return "res = (qb_compare_array_$type(op1_start, op1_end, op2_start, op2_end) == -1);";
	}
	
	protected function getScalarExpression() {
		return "res = (op1 < op2);";
	}
}

?>