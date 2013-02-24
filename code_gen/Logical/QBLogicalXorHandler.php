<?php

class QBLogicalXorHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return ($i == 3) ? "VAR" : $this->addressMode;
	}

	protected function getScalarExpression() {
		return "res = !op1 != !op2;";
	}
}

?>