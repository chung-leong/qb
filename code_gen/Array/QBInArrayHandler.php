<?php

class QBInArrayHandler extends QBArraySearchHandler {
	
	protected function getArrayExpression() {
		return "res = qb_find_{$this->operandType}(op1_ptr, op1_count, op2_ptr, op2_count) != -1;";
	}
	
	protected function getScalarExpression() {
		return "res = qb_find_{$this->operandType}(op1_ptr, op1_count, op2_ptr, 1) != -1;";
	}
}

?>