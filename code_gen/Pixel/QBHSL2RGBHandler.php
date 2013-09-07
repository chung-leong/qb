<?php

class QBHSL2RGBHandler extends QBColorSpaceConversionHandler {

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_convert_hsl_to_rgb_$type($cType *op1_ptr, $cType *res_ptr) {",
					"$cType h = op1_ptr[0], s = op1_ptr[1], l = op1_ptr[2];",
					"$cType f, c, r, g, b;",
					"int32_t i;",
					"if(s == 0) {",
						"r = g = b = l;",
					"} else {",
						"c = (1 - fabs$f(2 * l - 1)) * s;", 
						"h /= 60;",
						"i = qb_quick_floor$f(h);",
						"f = h - i;",
						"switch_loop:",
						"switch(i) {",
							"case 0:",
								"r = l + c * 0.5$f;",
								"g = l - c * 0.5$f + c * f;",
								"b = l - c * 0.5$f;",
								"break;",
							"case 1:",
								"r = l + c * 0.5$f - c * f;",
								"g = l + c * 0.5$f;",
								"b = l - c * 0.5$f;",
								"break;",
							"case 2:",
								"r = l - c * 0.5$f;",
								"g = l + c * 0.5$f;",
								"b = l - c * 0.5$f + c * f;",
								"break;",
							"case 3:",
								"r = l - c * 0.5$f;",
								"g = l + c * 0.5$f - c * f;",
								"b = l + c * 0.5$f;",
								"break;",
							"case 4:",
								"r = l - c * 0.5$f + c * f;",
								"g = l - c * 0.5$f;",
								"b = l + c * 0.5$f;",
								"break;",
							"case 5:",
								"r = l + c * 0.5$f;",
								"g = l - c * 0.5$f;", 
								"b = l + c * 0.5$f - c * f;",
								"break;",
							"default:",
								"i = i % 6;",
								"if(i < 0) {",
									"i += 6;",
								"}",
								"goto switch_loop;",
						"}",
					"}",
					"res_ptr[0] = r;",
					"res_ptr[1] = g;",
					"res_ptr[2] = b;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$lines = array();
		switch($this->operandSize) {
			case 3: 
				$lines[] = "qb_convert_hsl_to_rgb_$type(op1_ptr, res_ptr);";
			break;
			case 4:
				$lines[] = "qb_convert_hsl_to_rgb_$type(op1_ptr, res_ptr);";
				$lines[] = "res_ptr[3] = op1_ptr[3];";
			break;
		} 
		return $lines;
	}
}

?>