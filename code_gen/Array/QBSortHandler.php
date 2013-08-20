<?php

class QBSortHandler extends QBHandler {

	protected $direction = "ascending";

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "VAR";
			case 2: return "ARR";
		}
	}
	
	public function getFunctionType() {
		return "extern";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$dir = $this->direction;

		if($this->direction == 'ascending') {
			$scalarParams = "const void *a, const void *b";
		} else {
			$scalarParams = "const void *b, const void *a";
		}
		if(self::$compiler == 'MSVC') {
			// qsort_s() expects the extra param to come first
			$arrayParams = "void *extra, $scalarParams";
		} else {
			// qsort_r() expects the extram param to come last
			$arrayParams = "$scalarParams, const void *extra";			
		}
		$functions = array(
			array(
				"int qb_compare_{$dir}_{$type}($scalarParams) {",
					"if(*(($cType *) a) < *(($cType *) b)) {",
						"return -1;",
					"} else if(*(($cType *) a) > *(($cType *) b)) {",
						"return 1;",
					"} else {",
						"return 0;",
					"}",
				"}",
			),
			array(
				"int qb_compare_{$dir}_{$type}_array($arrayParams) {",
					"uint32_t len = *((const uint32_t *) extra);",
					"return qb_compare_array_$type(($cType *) a, len, ($cType *) b, len);",
				"}",
			),
		);
		return $functions;
	}

	public function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$dir = $this->direction;
		if(self::$compiler == 'MSVC') {
			$qsort_plus = 'qsort_s';
		} else {
			$qsort_plus = 'qsort_r';
		}
		$lines = array();
		$lines[] = "if(op1 == 1) {";
		$lines[] = 		"qsort(res_ptr, res_count, sizeof($cType) * 1, qb_compare_{$dir}_{$type});";
		$lines[] = "} else {";		
		$lines[] = 		"$qsort_plus(res_ptr, res_count / op1, sizeof($cType) * op1, qb_compare_{$dir}_{$type}_array, &op1);";
		$lines[] = "}";
		return $lines;
	}
}

?>