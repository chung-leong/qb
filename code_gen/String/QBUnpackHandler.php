<?php

class QBUnpackHandler extends QBHandler {
	protected $byteOrder;

	public function __construct($name, $operandType, $addressMode, $byteOrder) {
		$this->byteOrder = $byteOrder;
		parent::__construct($name, $operandType, $addressMode);
	}
	
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
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$width = intval(substr($type, 1));
		$macro = "SWAP_{$this->byteOrder}_I{$width}";
		return "*((uint{$width}_t *) &res) = $macro(*((uint{$width}_t *) op1_ptr));";
	}
}

?>