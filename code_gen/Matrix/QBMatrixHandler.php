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
			return "MATRIX2_ROWS";
		} else {
			return $this->operandSize;
		}
	}

}