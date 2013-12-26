<?php

class BranchTable extends Handler {

	use MultipleAddressMode, NoUnsigned, BranchInstruction;
	
	protected $tableSize;

	public function getInputOperandCount() {
		return 2;
	}
	
	public function __construct($baseName, $operandType, $addressMode, $size) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = 1;
		$this->addressMode = $addressMode;
		$this->tableSize = $size;
		$this->multipleData = false;		
	}

	public function getJumpTargetCount() {
		return $this->tableSize + 1;
	}

	public function getAction() {
		$lines = array();
		$lines[] = "offset = (unsigned int) (op1 - op2);";
		$lines[] = "if(UNEXPECTED(offset >= $this->tableSize)) {";
		$lines[] = 		"offset = $this->tableSize;";
		$lines[] = "}";
		return $lines;
	}
}

?>