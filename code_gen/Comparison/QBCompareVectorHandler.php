<?php

abstract class QBCompareVectorHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandType($i) {
		return ($i == 3) ? "I32" : $this->operandType;
	}
}

?>