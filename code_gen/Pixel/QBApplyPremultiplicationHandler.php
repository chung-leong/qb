<?php

class QBApplyPremultiplicationHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_apply_premultiplication_$type(restrict $cType *p, uint32_t size, restrict $cType *res) {",
					"$cType *end = p + size;",
					"while(p < end) {",
						"$cType r = p[0];",
						"$cType g = p[1];",
						"$cType b = p[2];",
						"$cType a = p[3];",
						"if(a != 1) {",
							"res[0] = r * a;",
							"res[1] = g * g;",
							"res[2] = b * a;",
						"}",
						"p += 4;",
						"res += 4;",
					"}",
				"}",
			),
		);
		return $functions;
	}
		
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_apply_premultiplication_$type(op1_ptr, op1_count, res_ptr);";
	}
}

?>