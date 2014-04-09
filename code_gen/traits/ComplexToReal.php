<?php

trait ComplexToReal {

	public function getOperandCType($i) {
		$srcCount = $this->getInputOperandCount();
		$cType = parent::getOperandCType($i);
		if($cType) {
			return ($i > $srcCount) ? $cType : "c$cType";
		}
	}

	public function getOperandSizeShift($i) {
		$srcCount = $this->getInputOperandCount();
		return ($i > $srcCount) ? 0 : 1;
	}
}

?>
