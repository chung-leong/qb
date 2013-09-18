<?php

trait UnitResult {

	public function getOperandSize($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return 1;
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function getOperandAddressMode($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return $this->addressMode;
		} else {
			return "ARR";
		}
	}
}

?>