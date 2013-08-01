<?php

class QBLogicalNotHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return ($i == 2) ? "VAR" : $this->addressMode;
	}

	protected function getActionForUnitData() {
		return "res = !op1;";
	}
}

?>