<?php

abstract class QBCompareHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return ($i == 3) ? "VAR" : $this->addressMode;
	}

	public function getOperandType($i) {
		return ($i == 3) ? "I32" : $this->operandType;
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if(!preg_match('/^I/', $type)) {
			$functions = array(
				array(
					"static int32_t ZEND_FASTCALL qb_compare_array_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end) {",
						"if(op1_start != op1_end && op2_start != op2_end) {",
							"$cType *op1_ptr = op1_start, *op2_ptr = op2_start;",
							"int32_t end1 = 0, end2 = 0;",
							"while(1) {",
								"if(*op1_ptr > *op2_ptr) {",
									"return 1;",
								"} else if(*op1_ptr < *op2_ptr) {",
									"return -1;",
								"}",
								"op1_ptr++;",
								"if(op1_ptr == op1_end) {",
									"if(end2) {",
										"break;",
									"}",
									"op1_ptr = op1_start;",
									"end1 = 1;",
								"}",
								"op2_ptr++;",
								"if(op2_ptr == op2_end) {",
									"if(end1) {",
										"break;",
									"}",
									"op2_ptr = op2_start;",
									"end2 = 1;",
								"}",
							"}",
						"} else if(op2_start != op2_end) {",
							"return -1;",
						"} else if(op1_start != op1_end) {",
							"return 1;",
						"}",
						"return 0;",
					"}",
				),
			);
			return $functions;
		}
	}	
}

?>