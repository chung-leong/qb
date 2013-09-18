<?php

trait ArrayComparisonOperator {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if(!preg_match('/^I/', $type)) {
			$functions = array(
				array(
					"int32_t ZEND_FASTCALL qb_compare_array_$type($cType *op1_ptr, uint32_t op1_count, $cType *op2_ptr, uint32_t op2_count) {",
						"if(op1_count && op2_count) {",
							"$cType *op1_start = op1_ptr, *op1_end = op1_ptr + op1_count;",
							"$cType *op2_start = op2_ptr, *op2_end = op2_ptr + op2_count;",
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
						"} else if(op2_count) {",
							"return -1;",
						"} else if(op1_count) {",
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