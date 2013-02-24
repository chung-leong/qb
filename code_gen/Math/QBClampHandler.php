<?php

class QBClampHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(3);
		$lines = array();
		$lines[] = "if(op3 < op1) {";
		$lines[] =		"res = op1;";
		$lines[] = "} else if(op3 > op2) {";
		$lines[] =		"res = op2;";
		$lines[] = "} else {";
		$lines[] = 		"res = op3;";
		$lines[] = "}";
		return $lines;;
	}
}

?>