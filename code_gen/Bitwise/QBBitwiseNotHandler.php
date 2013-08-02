<?php

class QBBitwiseNotHandler extends QBHandler {

	protected function getActionOnUnitData() {
		return "res = ~op1;";
	}
}

?>