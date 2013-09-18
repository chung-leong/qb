<?php

trait ArrayAddressMode {

	public function __construct($baseName, $operandType = NULL, $operandSize = 1, $multipleData = false) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = $operandSize;
		$this->addressMode = null;
		$this->multipleData = $multipleData;
	}
	
	public function isVectorized() {
		return false;
	}
}

?>