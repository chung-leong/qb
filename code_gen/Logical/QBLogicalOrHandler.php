<?php

// NOTE: employed only in Pixel Bender programs 

class QBLogicalOrHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return ($i == 3) ? "VAR" : $this->addressMode;
	}

	protected function getScalarExpression() {
		return "res = op1 || op2;";
	}
}

?>