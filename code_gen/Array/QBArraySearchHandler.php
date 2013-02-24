<?php

class QBArraySearchHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		if($i == 1) {
			return "ARR";
		} else if($i == 2) {
			return $this->addressMode;
		} else if($i == 3) {
			return "VAR";
		}
	}
	
	public function getOperandType($i) {
		if($i == 3) {
			return "I32";
		} else {
			return $this->operandType;
		}
	}

	protected function getArrayExpression() {
		return "res = qb_find_{$this->operandType}(op1_ptr, op1_count, op2_ptr, op2_count);";
	}
	
	protected function getScalarExpression() {
		return "res = qb_find_{$this->operandType}(op1_ptr, op1_count, op2_ptr, 1);";
	}
}

?>