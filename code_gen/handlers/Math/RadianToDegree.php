<?php

class RadianToDegree extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly, Multithreaded;

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		return "res = op1 * (($cType) (180 / M_PI));";
	}
}

?>