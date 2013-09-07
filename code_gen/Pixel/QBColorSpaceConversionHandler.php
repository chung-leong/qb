<?php

class QBColorSpaceConversionHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function needsUnrolling() {
		return false;
	}
}

?>