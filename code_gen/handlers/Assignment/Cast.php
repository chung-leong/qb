<?php

class Cast extends Handler {

	use MultipleAddressMode, UnaryOperator, ExpressionReplication;
	
	public function getOperandType($i) {
		list($srcType, $dstType) = explode('_', $this->operandType);
		switch($i) {
			case 1: return $srcType;
			case 2: return $dstType;
		}
	}
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		return "res = ($cType) op1;";
	}
}

?>