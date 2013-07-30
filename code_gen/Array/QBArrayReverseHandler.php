<?php

class QBArrayReverseHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "VAR";
			case 3: return "VAR";
			case 4: return "ARR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U32";
			case 4: return $this->operandType;
		}
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_reverse_array_elements_$type($cType *elements, uint32_t count, uint32_t width, $cType *res_ptr) {",
					"uint32_t i, j;",
					"if(width == 1) {",
						"for(i = 0, j = count - 1; i < ((count + 1) & ~1) / 2; i++, j--) {",
							"$cType t = elements[i];",
							"res_ptr[i] = elements[j];",
							"res_ptr[j] = t;",
						"}",
					"} else {",
						"ALLOCA_FLAG(use_heap)",
						"$cType *t = do_alloca(width * sizeof($cType), use_heap);",
						"for(i = 0, j = count - 1; i < ((count + 1) & ~1) / 2; i++, j--) {",
							"memcpy(t, &elements[i * width], width * sizeof($cType));",
							"memcpy(&res_ptr[i * width], &elements[j * width], width * sizeof($cType));",
							"memcpy(&res_ptr[j * width], t, width * sizeof($cType));",
						"}",
						"free_alloca(t, use_heap);",
					"}",
				"}",
			),
		);
		return $functions;
	}

	public function getAction() {
		return "qb_reverse_array_elements_{$this->operandType}(op1_ptr, op2, op3, res_ptr);";
	}	
}

?>