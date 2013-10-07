<?php

class ArrayIntersectCount extends Handler {

	use ScalarAddressMode, TernaryOperator, ArrayComparison;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "U32";
			case 4: return "U32";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "SCA";
			case 4: return "SCA";
		}
	}
		
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if($type[0] == 'I') {
			$signedType = 'S' . substr($type, 1);
		} else {
			$signedType = $type;
		}
		$lines = array();
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "$cType *op2_end = op2_ptr + op2_count, *op2_start = op2_ptr;";
		$lines[] = "uint32_t count = 0;";
		$lines[] = "if(op3 == 1) {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] = 			"int32_t found = FALSE;";
		$lines[] = 			"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr++) {";							
		$lines[] = 				"if(*op2_ptr == *op1_ptr) {";
		$lines[] = 					"found = TRUE;";
		$lines[] = 						"break;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 			"if(found) {";
		$lines[] = 				"count++;";
		$lines[] = 			"}";
		$lines[] = 			"op1_ptr++;";
		$lines[] = 		"}";
		$lines[] = "} else {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] = 			"int32_t found = FALSE;";
		$lines[] = 			"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr += op3) {";
		$lines[] = 				"if(qb_compare_array_$signedType(op1_ptr, op3, op2_ptr, op3) == 0) {";
		$lines[] = 					"found = TRUE;";
		$lines[] = 					"break;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 			"if(found) {";
		$lines[] = 				"count += op3;";
		$lines[] = 			"}";
		$lines[] = 			"op1_ptr += op3;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = count;";
		return $lines;
	}
}

?>