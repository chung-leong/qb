<?php

class UnpackLittleEndian extends Handler {

	use MultipleAddressMode, UnaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1:	return "U08";
			case 2: return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		$width = substr($this->operandType, 1);
		$macro = "SWAP_BE_I{$width}";
		return "*((uint{$width}_t *) &res) = $macro(*((uint{$width}_t *) op1_ptr));";
	}
}

?>