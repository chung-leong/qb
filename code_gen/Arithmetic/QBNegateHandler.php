<?php

class QBNegateHandler extends QBHandler {

	protected function getActionForUnitData() {
		return "res = - op1;";
	}
}

?>