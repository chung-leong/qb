<?php

class HSV2RGB extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void qb_convert_hsv_to_rgb_$type($cType *op1_ptr, $cType *res_ptr) {",
					"$cType h = op1_ptr[0], s = op1_ptr[1], v = op1_ptr[2];",
					"$cType f, c, r, g, b;",
					"int32_t i;",
					"if(s == 0) {",
						"r = g = b = v;",
					"} else {",
						"c = v * s;",
						"h /= 60;",
						"i = (int32_t) floor$f(h);",
						"f = h - i;",
						"switch_loop:",
						"switch(i) {",
							"case 0:",
								"r = v;",
								"g = v + c * (f - 1);",
								"b = v - c;",
								"break;",
							"case 1:",
								"r = v - c * f;",
								"g = v;",
								"b = v - c;",
								"break;",
							"case 2:",
								"r = v - c;",
								"g = v;",
								"b = v + c * (f - 1);",
								"break;",
							"case 3:",
								"r = v - c;",
								"g = v - c * f;",
								"b = v;",
								"break;",
							"case 4:",
								"r = v + c * (f - 1);",
								"g = v - c;",
								"b = v;",
								"break;",
							"case 5:",
								"r = v;",
								"g = v - c;", 
								"b = v - c * f;",
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
				$lines[] = "qb_convert_hsv_to_rgb_$type(op1_ptr, res_ptr);";
			break;
			case 4:
				$lines[] = "qb_convert_hsv_to_rgb_$type(op1_ptr, res_ptr);";
				$lines[] = "res_ptr[3] = op1_ptr[3];";
			break;
		} 
		return $lines;
	}
}

?>