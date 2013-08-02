<?php

class QBNegateHandler extends QBHandler {

	protected function getActionOnUnitData() {
		return "res = - op1;";
	}
}

?>