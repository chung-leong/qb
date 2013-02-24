<?php

class QBNormalize1xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getAction() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) (op1 / fabs(op1));";
	}

}

?>