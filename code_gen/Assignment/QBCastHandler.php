<?php

class QBCastHandler extends QBHandler {
	public $operandType1;
	public $operandType2;

	public function __construct($name, $operandType1, $operandType2, $addressMode, $vectorWidth = null) {
		$this->operandType1 = $operandType1;
		$this->operandType2 = $operandType2;
		parent::__construct($name, NULL, $addressMode, $vectorWidth);
	}

	public function getOperandType($i) {
		return ($i == 1) ? $this->operandType1 : $this->operandType2;
	}

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(2);
		if($this->operandType2[0] == 'F' && $this->operandType1[0] != 'F') {
			if(intval(substr($this->operandType1, 1)) != 64) {
				// convert floating point number to int64 first
				return "res = ($cType) (int64_t) op1;";
			}
		}
		return "res = ($cType) op1;";
	}
}

?>