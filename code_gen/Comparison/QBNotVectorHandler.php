<?php

class QBNotVectorHandler extends QBCompareVectorHandler {

	public function getInputOperandCount() {
		return 1;
	}

	protected function getActionOnUnitData() {
		return "res = (!op1);";
	}
}

?>