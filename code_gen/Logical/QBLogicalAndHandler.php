<?php

// NOTE: employed only in Pixel Bender programs 

class QBLogicalAndHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return ($i == 3) ? "VAR" : $this->addressMode;
	}

	protected function getActionForUnitData() {
		return "res = op1 && op2;";
	}
}

?>