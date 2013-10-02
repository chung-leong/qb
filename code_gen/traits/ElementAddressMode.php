<?php

trait ElementAddressMode {

	public function __construct($baseName, $operandType) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = 1;
		$this->addressMode = "ELE";
		$this->multipleData = false;
	}
}

?>