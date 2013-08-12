<?php

abstract class QBSampleHandler extends QBHandler {

	public function getInputOperandCount() {
		return 5;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: // image
			case 6: // result
				return "ARR";
			case 2:	// width
			case 3:	// height
				return "VAR";
			case 4: // x
			case 5: // y
				return $this->addressMode;
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: // image
			case 4: // x
			case 5: // y
			case 6: // result
				return $this->operandType;
			case 2:	// width
			case 3:	// height
				return "U32";
		}
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: 
			case 2:
			case 3: return 0;
			case 4: 
			case 5: return 1;
			default: return $this->operandSize;
		}
	}
	
	public function getFunctionType() {
		if(substr($this->operandType, 1) == 64) {
			// sampling at double precision is included for completeness only--don't inline any handler
			return "extern";
		} else {
			return parent::getFunctionType();
		}
	}
	
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
	
	public function needsUnrolling() {
		return false;
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return array("op4_count * {$this->operandSize}", "op5_count * {$this->operandSize}");
		} else {
			return $this->operandSize;
		}
	}
}

?>