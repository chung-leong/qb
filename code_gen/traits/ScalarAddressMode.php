<?php

trait ScalarAddressMode {

	public function __construct($baseName, $operandType) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = 1;
		$this->addressMode = "SCA";
		$this->multipleData = false;
	}
}

?>