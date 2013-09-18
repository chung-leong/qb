<?php

class Pack extends Handler {

	use MultipleAddressMode, UnaryOperator;

	protected $byteOrder;

	public function getOperandType($i) {
		list($type, $byteOrder) = explode('_', $this->operandType);
		switch($i) {
			case 1: return $type;
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
		list($type, $byteOrder) = explode('_', $this->operandType);
		$cType = $this->getOperandCType(1);
		$width = intval(substr($type, 1));
		$macro = "SWAP_{$byteOrder}_I{$width}";
		if($type[0] == 'F') {
			// accommodate native compiler, for which op1 might be a macro defined as a literal
			$lines = array();
			$lines[] = "$cType v = op1;";
			$lines[] = "*((uint{$width}_t *) res_ptr) = $macro(*((uint{$width}_t *) &v));";
			return $lines;
		} else {
			return "*((uint{$width}_t *) res_ptr) = $macro(op1);";
		}
	}
}

?>