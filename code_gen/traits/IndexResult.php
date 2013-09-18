<?php

trait IndexResult {

	public function getOperandType($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return "U32";
		} else {
			return parent::getOperandType($i);
		}
	}
}

?>