<?php

class QBLessThanVectorHandler extends QBCompareVectorHandler {

	protected function getActionForUnitData() {
		return "res = (op1 < op2);";
	}
}

?>