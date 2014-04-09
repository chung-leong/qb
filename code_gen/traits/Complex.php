<?php

trait Complex {

	public function getOperandCType($i) {
		$cType = parent::getOperandCType($i);
		if($cType) {
			return "c$cType";
		}
	}

	public function getOperandSizeShift($i) {
		return 1;
	}
}

?>
