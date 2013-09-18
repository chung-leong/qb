<?php

trait ScalarResult {

	public function getOperandAddressMode($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return "SCA" : 
		} else {
			return parent::getOperandAddressMode($i);
		}
	}
}

?>