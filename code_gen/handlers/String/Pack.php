<?php

class Pack extends Handler {

	use MultipleAddressMode, UnaryOperator;

	protected $byteOrder;

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

	protected function getActionOnUnitData() {
		list($name, $byteOrder) = explode('_', $this->baseName);
		$cType = $this->getOperandCType(1);
		$width = substr($this->operandType, 1);
		$macro = "SWAP_{$byteOrder}_I{$width}";
		if($this->operandType[0] == 'F') {
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