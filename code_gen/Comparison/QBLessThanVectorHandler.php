<?php

class QBLessThanVectorHandler extends QBCompareVectorHandler {

	protected function getActionOnUnitData() {
		return "res = (op1 < op2);";
	}
}

?>