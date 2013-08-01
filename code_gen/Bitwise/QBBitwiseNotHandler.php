<?php

class QBBitwiseNotHandler extends QBHandler {

	protected function getActionForUnitData() {
		return "res = ~op1;";
	}
}

?>