<?php

class QBArrayDifferenceHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "U32";
			case 4: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "VAR";
			case 4: return "ARR";
		}
	}
		
	public function getResultSizePossibilities() {
		return "vector_count";
	}
	
	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		$lines = array();
		$lines[] = "vector_count = qb_get_difference_count_{$type}(op1_ptr, op1_count, op2_ptr, op2_count, op3);";
		return $lines;
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if($type[0] == 'I') {
			$signedType = 'S' . substr($type, 1);
		} else {
			$signedType = $type;
		}
		$functions = array(
			array(
				"uint32_t qb_get_difference_count_{$type}($cType *op1_ptr, uint32_t op1_count, $cType *op2_ptr, uint32_t op2_count, uint32_t op3) {",
					"$cType *op1_end = op1_ptr + op1_count;",
					"$cType *op2_end = op2_ptr + op2_count, *op2_start = op2_ptr;",
					"uint32_t count = 0;",
					"if(op3 == 1) {",
						"while(op1_ptr < op1_end) {",
							"int32_t found = FALSE;",
							"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr++) {",							
								"if(*op2_ptr == *op1_ptr) {",
									"found = TRUE;",
									"break;",
								"}",
							"}",
							"if(!found) {",
								"count++;",
							"}",
							"op1_ptr++;",
						"}",
					"} else {",
						"while(op1_ptr < op1_end) {",
							"int32_t found = FALSE;",
							"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr += op3) {",
								"if(qb_compare_array_$signedType(op1_ptr, op3, op2_ptr, op3) == 0) {",
									"found = TRUE;",
									"break;",
								"}",
							"}",
							"if(!found) {",
								"count += op3;",
							"}",
							"op1_ptr += op3;",
						"}",
					"}",
					"return count;",
				"}",
			),
		);
		return $functions;
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
		$lines[] = "if(op3 == 1) {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] =			"int32_t found = FALSE;";
		$lines[] = 			"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr++) {";
		$lines[] = 				"if(*op2_ptr == *op1_ptr) {";
		$lines[] =					"found = TRUE;";
		$lines[] =					"break;";
		$lines[] =				"}";
		$lines[] =			"}";
		$lines[] =			"if(!found) {";
		$lines[] = 				"*res_ptr = *op1_ptr;";
		$lines[] = 				"res_ptr++;";
		$lines[] =			"}";
		$lines[] =			"op1_ptr++;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] =		"while(op1_ptr < op1_end) {";
		$lines[] =			"int32_t found = FALSE;";
		$lines[] =			"for(op2_ptr = op2_start; op2_ptr < op2_end; op2_ptr += op3) {";
		$lines[] =				"if(qb_compare_array_$signedType(op1_ptr, op3, op2_ptr, op3) == 0) {";
		$lines[] =					"found = TRUE;";
		$lines[] =					"break;";
		$lines[] =				"}";
		$lines[] =			"}";
		$lines[] =			"if(!found) {";
		$lines[] =				"memcpy(res_ptr, op1_ptr, sizeof($cType) * op3);";
		$lines[] = 				"res_ptr += op3;";
		$lines[] =			"}";
		$lines[] =			"op1_ptr += op3;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>