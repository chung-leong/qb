<?php

class QBNotEqualVectorHandler extends QBCompareVectorHandler {

	protected function getActionOnUnitData() {
		return "res = (op1 != op2);";
	}
}

?>