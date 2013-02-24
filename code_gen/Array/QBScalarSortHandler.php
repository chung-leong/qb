<?php

class QBScalarSortHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"int qb_compare_ascending_$type(const void *a, const void *b) {",
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
				"static void ZEND_FASTCALL qb_sort_ascending_$type(qb_interpreter_context *cxt, $cType *elements, uint32_t count) {",
					"qsort(elements, count, sizeof($cType), qb_compare_ascending_$type);",
				"}",
			),
		);
		return $functions;
	}

	public function getAction() {
		return "qb_sort_ascending_{$this->operandType}(cxt, res_ptr, res_count);";
	}
}

?>