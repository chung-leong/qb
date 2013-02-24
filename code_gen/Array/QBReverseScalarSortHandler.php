<?php

class QBReverseScalarSortHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
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
			array(
				"static void ZEND_FASTCALL qb_sort_descending_$type(qb_interpreter_context *cxt, $cType *elements, uint32_t count) {",
					"qsort(elements, count, sizeof($cType), qb_compare_descending_$type);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_sort_descending_$type(cxt, res_ptr, res_count);";
	}
}

?>