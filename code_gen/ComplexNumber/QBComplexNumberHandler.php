<?php

class QBComplexNumberHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getOperandSize($i) {
		return "2";
	}
	
	public function getfunctionType() {
		if(substr($this->operandType, 1) == 64) {
			// if it's double precision, overhead from function call probably doesn't matter
			return "extern";
		} else {
			return parent::getFunctionType();
		}
	}	
}

?>