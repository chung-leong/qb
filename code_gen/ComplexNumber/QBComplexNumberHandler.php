<?php

class QBComplexNumberHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getOperandSize($i) {
		return "2";
	}
	
	public function getAction() {
		$expr = $this->getComplexNumberExpression();
		return ($this->addressMode == "ARR") ? $this->getIterationCode($expr) : $expr;
	}	
}

?>