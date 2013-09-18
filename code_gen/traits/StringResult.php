<?php

trait StringResult {

	public function getOperandType($i) {
		$srcCount = $this->getInputOperandCount();
		if($i > $srcCount) {
			return "U08";
		} else {
			return parent::getOperandType($i);
		}
	}
}

?>