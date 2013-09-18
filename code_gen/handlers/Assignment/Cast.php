<?php

class Cast extends Handler {
	public $sourceOperandType;

	public function __construct($name, $operandType1, $operandType2, $addressMode, $vectorWidth = null) {
		$this->baseName = $baseName;
		$this->sourceOperandType = $operandType1;
		$this->operandType = $operandType2;
		$this->operandSize = $operandSize;
		$this->addressMode = $addressMode;
		$this->multipleData = ($addressMode == "ARR");
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->sourceOperandType;
			case 2: return $this->operandType;
		}
	}
	
	public function getFunctionName() {
		$name = parent::getFunctionName();
		$name .= "_{$this->operandType1}_{$this->operandType2}";
		return $name;
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		return "res = ($cType) op1;";
	}
}

?>