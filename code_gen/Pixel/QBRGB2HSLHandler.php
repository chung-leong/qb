<?php

class QBRGB2HSLHandler extends QBColorSpaceConversionHandler {

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_convert_rgb_to_hsl_$type($cType *op1_ptr, $cType *res_ptr) {",
					"$cType r = op1_ptr[0], g = op1_ptr[1], b = op1_ptr[2];",
					"$cType min, max, delta, h, s, l;",
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
					"l = (max + min) * 0.5$f;",
					"delta = max - min;",
					"if(delta != 0) {",
						"s = delta / (1 - fabs$f(max + min - 1));",
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
					"res_ptr[2] = l;",
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
				$lines[] = "qb_convert_rgb_to_hsl_$type(op1_ptr, res_ptr);";
			break;
			case 4:
				$lines[] = "qb_convert_rgb_to_hsl_$type(op1_ptr, res_ptr);";
				$lines[] = "res_ptr[3] = op1_ptr[3];";
			break;
		} 
		return $lines;
	}
}

?>