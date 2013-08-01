<?php

class QBNotVectorHandler extends QBCompareVectorHandler {

	public function getInputOperandCount() {
		return 1;
	}

	protected function getActionForUnitData() {
		return "res = (!op1);";
	}
}

?>