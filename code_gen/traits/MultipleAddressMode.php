<?php

trait MultipleAddressMode {

	public function __construct($baseName, $operandType, $addressMode, $operandSize = 1, $multipleData = null) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = $operandSize;
		$this->addressMode = $addressMode;
		if($multipleData) {
			$this->multipleData = true;
		} else {
			if($multipleData === null) {
				if($addressMode == "ARR") {
					// if array addressing is used where the operands are only 1 across, 
					// then it's a multiple-input-output situation
					if($operandSize == 1) {
						$this->multipleData = true;
					}
				}
			}
		}
	}
	
	public function disableMultipleData() {
		// change the address mode to SCA, unless the handler is unrolling a scalar expression
		// in which case the address mode needs to stay ARR
		if(!$this->needsUnrolling()) {
			$this->addressMode = "SCA";
		}
		$this->multipleData = false;
	}
	
	public function restoreMultipleData() {
		$this->addressMode = "ARR";
		$this->multipleData = true;
	}
}

?>