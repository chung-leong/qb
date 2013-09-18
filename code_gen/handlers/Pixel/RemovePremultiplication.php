<?php

class RemovePremultiplication extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		$type = $this->getOperandType(2);
		$lines = array();
		$lines[] = "$cType a = op1_ptr[3];";
		$lines[] = "if(a != 1) { ";
		$lines[] = 		"$cType ia = (a != 0) ? 1 / a : 0;";
		$lines[] = 		"res_ptr[0] = op1_ptr[0] * ia;";
		$lines[] = 		"res_ptr[1] = op1_ptr[1] * ia;";
		$lines[] = 		"res_ptr[2] = op1_ptr[2] * ia;";
		$lines[] = 		"res_ptr[3] = op1_ptr[3];";
		$lines[] = "} else {";
		$lines[] = 		"res_ptr[0] = op1_ptr[0];";
		$lines[] = 		"res_ptr[1] = op1_ptr[1];";
		$lines[] = 		"res_ptr[2] = op1_ptr[2];";
		$lines[] = 		"res_ptr[3] = op1_ptr[3];";
		$lines[] = "}";
		return $lines;
	}	
}

?>