<?php

class QBMatrixHandler extends QBHandler {

	protected $operandPadding = 0;

	public function __construct($name, $operandType, $addressMode, $vectorSize = "variable") {
		if(gettype($vectorSize) == 'string') {
			if(preg_match('/\\+P$/i', $vectorSize)) {
				$vectorSize = (int) $vectorSize;
				$this->operandPadding = 1;
			}
		}
		parent::__construct($name, $operandType, $addressMode, $vectorSize);
	}
	
	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			return "MATRIX1_COLS";
		} else {
			return $this->operandSize;
		}
	}
	
	public function getName() {
		$name = parent::getName();
		if($this->operandPadding) {
			$name = str_replace("{$this->operandSize}X", "{$this->operandSize}XP", $name);
		}
		return $name;
	}
	
	public function getFunctionName() {
		$name = parent::getFunctionName();
		if($this->operandPadding) {
			$name = str_replace("{$this->operandSize}x", "{$this->operandSize}x_padded", $name);
		}
		return $name;
	}
	
	public function needsUnrolling() {
		return false;
	}
	
	public function needsMatrixDimensions($which = null) {
		if($this->operandSize == "variable") {
			if(!$which || $which <= $this->getInputOperandCount()) {
				return true;
			}
		} 
		return false;
	}
}