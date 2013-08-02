<?php

class QBPackHandler extends QBHandler {
	protected $byteOrder;

	public function __construct($name, $operandType, $addressMode, $byteOrder) {
		$this->byteOrder = $byteOrder;
		parent::__construct($name, $operandType, $addressMode);
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2:	return "U08";
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return "ARR";
		}
	}

	public function getResultSizePossibilities() {
		$type = $this->getOperandType(1);
		$size = substr($type, 1) / 8;
		return $size;
	}

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$width = intval(substr($type, 1));
		$macro = "SWAP_{$this->byteOrder}_I{$width}";
		if($type[0] == 'F') {
			// accommodate native compiler, for which op1 might be a macro defined as a literal
			$lines = array();
			$lines[] = "{";
			$lines[] = 		"$cType v = op1;";
			$lines[] = 		"*((uint{$width}_t *) res_ptr) = $macro(*((uint{$width}_t *) &v));";
			$lines[] = "}";
			return $lines;
		} else {
			return "*((uint{$width}_t *) res_ptr) = $macro(op1);";
		}
	}
}

?>