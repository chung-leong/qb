<?php

trait PixelSampler {

	public function getInputOperandCount() {
		return 5;
	}

	public function getOutputOperandCount() {
		return 1;
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";	// image
			case 2:	return "SCA";	// width
			case 3:	return "SCA";	// height
			case 4: return $this->addressMode;	// x
			case 5: return $this->addressMode;	// y
			case 6: return "ARR";	// result
				
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;	// image
			case 2:	return "U32";	// width
			case 3:	return "U32";	// height
			case 4: return $this->operandType;	// x
			case 5: return $this->operandType;	// y
			case 6: return $this->operandType;	// result
		}
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: return 0;	// the image remains fixed during iteration
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
			case 5: return 1;
			case 6: return $this->operandSize;
		}
	}
}

?>