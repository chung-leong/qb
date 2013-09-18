<?php

trait BooleanResult {

	public function getOperandType($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return "I32";
		} else {
			return parent::getOperandType($i);
		}
	}
}

?>