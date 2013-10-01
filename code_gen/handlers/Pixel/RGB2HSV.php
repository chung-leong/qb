<?php

class RGB2HSV extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"void qb_convert_rgb_to_hsv_$type($cType *op1_ptr, $cType *res_ptr) {",
					"$cType r = op1_ptr[0], g = op1_ptr[1], b = op1_ptr[2];",
					"$cType min, max, delta, h, s, v;",
					"if(r > g) {",
						"max = r;",
						"min = g;",
					"} else {",
						"max = g;",
						"min = r;",
					"}",
					"if(b > max) {",
						"max = b;",
					"} else if(b < min) {",
						"min = b;",
					"}",
					"v = max;",
					"delta = max - min;",
					"if(delta != 0) {",
						"s = delta / max;",
						"if(r == max) {",
							"h = (g - b) / delta;",
						"} else if(g == max) {",
							"h = 2 + (b - r) / delta;",
						"} else {",
							"h = 4 + (r - g) / delta;",
						"}",
						"h *= 60;",
						"if(h < 0) {",
							"h += 360;",
						"}",
					"} else {",
						"s = 0;",
						"h = 0;",
					"}",
					"res_ptr[0] = h;",
					"res_ptr[1] = s;",
					"res_ptr[2] = v;",
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
				$lines[] = "qb_convert_rgb_to_hsv_$type(op1_ptr, res_ptr);";
			break;
			case 4:
				$lines[] = "qb_convert_rgb_to_hsv_$type(op1_ptr, res_ptr);";
				$lines[] = "res_ptr[3] = op1_ptr[3];";
			break;
		} 
		return $lines;
	}
}

?>