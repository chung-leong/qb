<?php

abstract class QBSampleHandler extends QBHandler {

	public function getInputOperandCount() {
		return 5;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: // image
			case 6: // result
				return "ARR";
			case 2:	// width
			case 3:	// height
				return "VAR";
			case 4: // x
			case 5: // y
				return $this->addressMode;
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: // image
			case 4: // x
			case 5: // y
			case 6: // result
				return $this->operandType;
			case 2:	// width
			case 3:	// height
				return "U32";
		}
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: 
			case 2:
			case 3: return 0;
			case 4: 
			case 5: return 1;
			default: return $this->operandSize;
		}
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return array("op4_count * {$this->operandSize}", "op5_count * {$this->operandSize}");
		} else {
			return $this->operandSize;
		}
	}
}

?>