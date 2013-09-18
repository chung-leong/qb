<?php

trait UseQuickRoundingFunction {

	public function getHelperFunctions() {
		$lines = array(
			array(
				"static zend_always_inline int32_t qb_quick_floor(double f) {",
				"#ifdef FAST_FLOAT_TO_INT",
					"return (int32_t) floor(f);",
				"#else",
					"int32_t n;",
					"if(f < -32768) {",
						"return -32768;",
					"} else if(f > 32767) {",
						"return 32767;",
					"}",
					"f += 103079215104.0;",
					"n = ((long *) &f)[0];",
					"return n >> 16;",
				"#endif",
				"}",
			),
			array(
				"static zend_always_inline int32_t qb_quick_round(double f) {",
				"#ifdef FAST_FLOAT_TO_INT",
					"return (int32_t) round(f);",
				"#else",
					"int32_t n;",
					"if(f < -32768) {",
						"return -32768;",
					"} else if(f > 32767) {",
						"return 32767;",
					"}",
					"f += 103079215103.5;",
					"n = ((long *) &f)[0];",
					"return n >> 16;",
				"#endif",
				"}",
			),
			array(
				"static zend_always_inline int32_t qb_quick_floorf(float f) {",
				"#ifdef FAST_FLOAT_TO_INT",
					"return (int32_t) floorf(f);",
				"#else",
					"return qb_quick_floor(f);",
				"#endif",
				"}",
			),
			array(
				"static zend_always_inline int32_t qb_quick_roundf(float f) {",
				"#ifdef FAST_FLOAT_TO_INT",
					"return (int32_t) roundf(f);",
				"#else",
					"return qb_quick_round(f);",
				"#endif",
				"}",	
			),
		);
		return $lines;
	}
}

?>