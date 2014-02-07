<?php

trait DoublePrecisionIsRare {

	// don't inline the handler if operand type is double precision
	public function getFunctionType() {
		$srcCount = $this->gerInputOperandCount();
		$type = $this->getOperandType($srcCount + 1);
		if(substr($type, 1) == 64) {
			return "extern";
		} else {
			return parent::getFunctionType();
		}
	}
}

?>