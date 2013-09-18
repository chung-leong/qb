<?php

trait MultipleAddressMode {

	public function __construct($baseName, $operandType, $addressMode, $operandSize = 1) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = $operandSize;
		$this->addressMode = $addressMode;
		if($addressMode == "ARR") {
			$this->multipleData = true;
		}
		if($operandSize > 1) {
			$this->unrolling = true;
		}
	}
	
	public function isVectorized() {
		return ($this->operandSize != 1);
	}
}

?>