<?php

class QBReverseScalarSortHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getFunctionType() {
		return "extern";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"int qb_compare_descending_$type(const void *a, const void *b) {",
					"if(*(($cType *) a) < *(($cType *) b)) {",
						"return 1;",
					"} else if(*(($cType *) a) > *(($cType *) b)) {",
						"return -1;",
					"} else {",
						"return 0;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "qsort(res_ptr, res_count, sizeof($cType), qb_compare_descending_$type);";
		return $lines;
	}
}

?>