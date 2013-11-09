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
		return (2 + $this->dimensionCount * 2 + 2);
	}
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function changesOperand($i) {
		if($i == 2) {
			return true;
		} else if($i <= 2 + $this->dimensionCount * 2) {
			if(!($i & 1)) {
				return true;
			}
		}
		return false;
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		if($i == 1) {
			return "U32";		// source size
		} else if($i == 2) {
			return "U32";		// destination size
		} else if($i <= 2 + $this->dimensionCount * 2) {
			if($i & 1) {
				return "U32";		// source dimension
			} else {
				return "U32";		// destination dimension
			}
		} else if($i <= 2 + $this->dimensionCount * 2 + 2) {
			if($i & 1) {
				return "U32";		// segment selector
			} else {
				return "U32";		// element byte-count
			}
		}
	}
	
	public function getOperandAddressMode($i) {
		if($i == 1) {
			return "SCA";
		} else if($i == 2) {
			return "SCA";
		} else if($i <= 2 + $this->dimensionCount * 2) {
			if($i & 1) {
				return "SCA";
			} else {
				return "SCA";
			}
		} else if($i <= 2 + $this->dimensionCount * 2 + 2) {
			if($i & 1) {
				return "CON";
			} else {
				return "CON";
			}
		}
	}

	protected function getFunctionNameComponents($prefix) {
		$parts = parent::getFunctionNameComponents($prefix);
		array_splice($parts, -1, 0, $this->dimensionCount);
		return $parts;
	}
	
	protected function getActionOnUnitData() {
		$segmentSelector = "op" . (2 + $this->dimensionCount * 2 + 1);
		$elementSize = "op" . (2 + $this->dimensionCount * 2 + 2);
		$lines = array();
		$lines[] =		"op2 = op1;";
		for($i = 3, $j = 4; $i < 2 + $this->dimensionCount * 2; $i += 2, $j += 2) {
			$lines[] =	"op$j = op$i;";
		}
		$lines[] = 		"qb_resize_segment(&cxt->function->local_storage->segments[$segmentSelector], op1 * $elementSize);";
		return $lines;
	}
}

?>