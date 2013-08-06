<?php

// only use in Pixel Bender (no unit-data version needed)

class QBApplyPremultiplicationHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandSize($i) {
		return 4;
	}

	public function isMultipleData() {
		return true;
	}

	public function getActionOnMultipleData() {
		$cType = $this->getOperandCType(2);
		$type = $this->getOperandType(2);
		$lines = array();
		$lines[] = "$cType a = op1_ptr[3];";
		$lines[] = "if(a != 1) {";
		$lines[] = 		"res_ptr[0] = op1_ptr[0] * a;";
		$lines[] = 		"res_ptr[1] = op1_ptr[1] * a;";
		$lines[] = 		"res_ptr[2] = op1_ptr[2] * a;";
		$lines[] = 		"res_ptr[3] = op1_ptr[3];";
		$lines[] = "} else {";
		$lines[] = 		"res_ptr[0] = op1_ptr[0];";
		$lines[] = 		"res_ptr[1] = op1_ptr[1];";
		$lines[] = 		"res_ptr[2] = op1_ptr[2];";
		$lines[] = 		"res_ptr[3] = op1_ptr[3];";
		$lines[] = "}";
		return $this->getIterationCode($lines);
	}
}

?>