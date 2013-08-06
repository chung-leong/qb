<?php

abstract class QBCompareVectorHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function isMultipleData() {
		return true;
	}
	
	public function getOperandType($i) {
		return ($i == 3) ? "I32" : $this->operandType;
	}
	
	public function getActionOnMultipleData() {
		$expr = $this->getActionOnUnitData();
		return $this->getIterationCode($expr);
	}
}

?>