<?php

class AccommodateSizeCopyDimension extends Handler {

	use MainThreadExecution;
	
	protected $dimensionCount;
	
	public function __construct($baseName, $operandType, $dimensionCount) {
		$this->baseName = $baseName;
		$this->operandType = $operandType;
		$this->operandSize = 1;
		$this->addressMode = "SCA";
		$this->multipleData = false;
		$this->dimensionCount = $dimensionCount;
	}
	
	public function getInputOperandCount() {
		return ($this->dimensionCount * 4 + 2);
	}
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function changesOperand($i) {
		if($i <= $this->dimensionCount * 4) {
			$j = $i % 4; 
			switch($j) {
				case 2:	return true;		// destination array size
				case 0:	return true;		// destination dimension
			}
		}
		return false;
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		if($i <= $this->dimensionCount * 4) {
			$j = $i % 4; 
			switch($j) {
				case 1:	return "U32";		// source array size
				case 2:	return "U32";		// destination array size
				case 3:	return "U32";		// source dimension
				case 0:	return "U32";		// destination dimension
			}
		} else {
			$j = $i - $this->dimensionCount * 4;
			switch($j) {
				case 1: return "U32";		// segment selector
				case 2: return "U32";		// element byte-count
			}
		}
	}
	
	public function getOperandAddressMode($i) {
		if($i <= $this->dimensionCount * 4) {
			$j = $i % 4; 
			switch($j) {
				case 1:	return "SCA";		// source array size
				case 2:	return "SCA";		// destination array size
				case 3:	return "SCA";		// source dimension
				case 0:	return "SCA";		// destination dimension
			}
		} else {
			$j = $i - $this->dimensionCount * 4;
			switch($j) {
				case 1: return "CON";		// segment selector
				case 2: return "CON";		// element byte-count
			}
		}
	}

	protected function getFunctionNameComponents($prefix) {
		$parts = parent::getFunctionNameComponents($prefix);
		array_splice($parts, -1, 0, $this->dimensionCount);
		return $parts;
	}
	
	protected function getActionOnUnitData() {
		$segmentSelector = "op" . ($this->dimensionCount * 4 + 1);
		$elementSize = "op" . ($this->dimensionCount * 4 + 2);
		$lines = array();
		for($i = 1, $j = 2; $i < $this->dimensionCount * 4; $i += 2, $j += 2) {
			$lines[] =	"op$j = op$i;";
		}
		$lines[] = 		"qb_resize_segment(&cxt->function->local_storage->segments[$segmentSelector], op1 * $elementSize);";
		return $lines;
	}
}

?>