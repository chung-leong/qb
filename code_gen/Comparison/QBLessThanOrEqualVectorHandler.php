<?php

class QBLessThanOrEqualVectorHandler extends QBCompareVectorHandler {

	protected function getActionForUnitData() {
		return "res = (op1 <= op2);";
	}
}

?>