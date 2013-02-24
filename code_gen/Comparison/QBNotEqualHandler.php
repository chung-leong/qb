<?php

class QBNotEqualHandler extends QBCompareHandler {

	protected function getArrayExpression() {
		$type = $this->getOperandType(1);
		$type = preg_replace('/^I/', 'S', $type);
		return "res = (qb_compare_array_$type(op1_start, op1_end, op2_start, op2_end) != 0);";
	}
	
	protected function getScalarExpression() {
		return "res = (op1 != op2);";
	}
}

?>