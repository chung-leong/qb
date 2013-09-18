<?php

trait NoOperands {

	public function __construct($baseName) {
		$this->baseName = $baseName;
		$this->operandType = null;
		$this->operandSize = 0;
		$this->addressMode = null;
		$this->multipleData = false;
	}
}

?>