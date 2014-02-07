<?php

class Gather extends Handler {

	use ArrayAddressMode, BinaryOperator, NoUnsigned;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";					
			case 2: return $this->operandType;
			case 3: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "CON";
			case 2: return "ARR";
			case 3: return "ARR";
		}
	}

	protected function getActionOnUnitData() {
		$lines = array();
		$temporaryVariables = array();
		for($i = 0; $i < $this->operandSize; $i++) {
			$temporaryVariables[] = "v$i";
		}
		$temporaryVariables = implode(", ", $temporaryVariables);
		$cType = $this->getOperandCType(2);
		$lines[] = "$cType $temporaryVariables;";
		for($i = 0; $i < $this->operandSize; $i++) {
			$shift = $i * 3;
			$lines[] = "v$i = op2_ptr[(op1 >> $shift) & 0x0007];";
		}
		for($i = 0; $i < $this->operandSize; $i++) {
			$lines[] = "res_ptr[$i] = v$i;";
		}
		return $lines;
	}
}

?>