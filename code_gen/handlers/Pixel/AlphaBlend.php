<?php

class AlphaBlend extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly, Multithreaded;

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		if($this->operandSize == 2) {
			$lines[] = "$cType a2 = op2_ptr[1];";
			$lines[] = "if(a2 != 0) {";
			$lines[] = 		"$cType a1 = op1_ptr[1];";
			$lines[] = 		"if(a1 != 0) {";
			$lines[] = 			"$cType y1 = op1_ptr[0];";
			$lines[] = 			"$cType y2 = op2_ptr[0];";
			$lines[] = 			"$cType af = a2 + a1 * (1 - a2);";
			$lines[] = 			"$cType iaf = 1 / af;";
			$lines[] = 			"res_ptr[0] = (y2 * a2 * iaf) + (y1 * a1 * iaf) * (1 - a2);";
			$lines[] = 			"res_ptr[1] = af;";
			$lines[] = 		"} else {";
			$lines[] = 			"res_ptr[0] = op2_ptr[0];";
			$lines[] = 			"res_ptr[1] = op2_ptr[1];";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"res_ptr[0] = op1_ptr[0];";
			$lines[] = 		"res_ptr[1] = op1_ptr[1];";
			$lines[] = "}";		
		} else {
			$lines[] = "$cType a2 = op2_ptr[3];";
			$lines[] = "if(a2 != 0) {";
			$lines[] = 		"$cType a1 = op1_ptr[3];";
			$lines[] = 		"if(a1 != 0) {";
			$lines[] = 			"$cType r1 = op1_ptr[0];";
			$lines[] = 			"$cType g1 = op1_ptr[1];";
			$lines[] = 			"$cType b1 = op1_ptr[2];";
			$lines[] = 			"$cType r2 = op2_ptr[0];";
			$lines[] = 			"$cType g2 = op2_ptr[1];";
			$lines[] = 			"$cType b2 = op2_ptr[2];";
			$lines[] = 			"$cType af = a2 + a1 * (1 - a2);";
			$lines[] = 			"$cType iaf = 1 / af;";
			$lines[] = 			"res_ptr[0] = (r2 * a2 * iaf) + (r1 * a1 * iaf) * (1 - a2);";
			$lines[] = 			"res_ptr[1] = (g2 * a2 * iaf) + (g1 * a1 * iaf) * (1 - a2);";
			$lines[] = 			"res_ptr[2] = (b2 * a2 * iaf) + (b1 * a1 * iaf) * (1 - a2);";
			$lines[] = 			"res_ptr[3] = af;";
			$lines[] = 		"} else {";
			$lines[] = 			"res_ptr[0] = op2_ptr[0];";
			$lines[] = 			"res_ptr[1] = op2_ptr[1];";
			$lines[] = 			"res_ptr[2] = op2_ptr[2];";
			$lines[] = 			"res_ptr[3] = op2_ptr[3];";
			$lines[] = 		"}";
			$lines[] = "} else {";
			$lines[] = 		"res_ptr[0] = op1_ptr[0];";
			$lines[] = 		"res_ptr[1] = op1_ptr[1];";
			$lines[] = 		"res_ptr[2] = op1_ptr[2];";
			$lines[] = 		"res_ptr[3] = op1_ptr[3];";
			$lines[] = "}";		
		}
		return $lines;
	}
}

?>