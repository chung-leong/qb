<?php

abstract class QBSampleHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: // image
			case 6: // result
				return "ARR";
			case 2:	// width
			case 3:	// height
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
	
	public function getResultSizePossibilities() {
		return 4;
	}
	
	protected function getOperandBoundCheckingType($i) {
		// no bound checking--code translator needs to make sure only valid address get passed (i.e. disallowing sub-arrays)
		return 0;
	}
}

?>