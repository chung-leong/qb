<?php

class QBSignHandler extends QBHandler {

	protected function getActionForUnitData() {
		$lines = array();
		$lines[] = "if(op1 < 0) {";
		$lines[] =		"res = -1;";
		$lines[] = "} else if(op1 > 0) {";
		$lines[] =		"res = 1;";
		$lines[] = "} else {";
		$lines[] = 		"res = 0;";
		$lines[] = "}";
		return $lines;;
	}
}

?>