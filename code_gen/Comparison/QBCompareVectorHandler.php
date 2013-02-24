<?php

abstract class QBCompareVectorHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandType($i) {
		return ($i == 3) ? "I32" : $this->operandType;
	}
	
	public function getAction() {
		// descendent handler always operates with vectors (i.e. arrays)
		return $this->getArrayExpression();
	}
}

?>