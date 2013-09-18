<?php

class Unpack extends Handler {

	use MultipleAddressMode, UnaryOperator;

	public function getOperandType($i) {
		list($type, $byteOrder) = explode('_', $this->operandType);
		switch($i) {
			case 1:	return "U08";
			case 2: return $type;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}
	
	public function getActionOnUnitData() {
		list($type, $byteOrder) = explode('_', $this->operandType);
		$cType = $this->getOperandCType(2);
		$width = intval(substr($type, 1));
		$macro = "SWAP_{$byteOrder}_I{$width}";
		return "*((uint{$width}_t *) &res) = $macro(*((uint{$width}_t *) op1_ptr));";
	}
}

?>