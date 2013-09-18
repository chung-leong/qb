<?php

trait ArrayResult {

	public function getOperandAddressMode($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return "ARR";
		} else {
			return parent::getOperandAddressMode($i);
		}
	}
}

?>