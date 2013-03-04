<?php

class QBConcatVariableHandler extends QBHandler {

	public function getOperandType($i) {
		// for now, $this->operandType should be going to be U08 as well
		return ($i == 2) ? "U08" : $this->operandType;
	}

	public function getOperandAddressMode($i) {
		// the result is an array
		return ($i == 2) ? "ARR" : $this->addressMode;
	}
	
	protected function getPrintExpression() {
		// use explicit cast to ensure any literal values are correctly pushed onto call stack
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, ($cType) op1)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, ($cType) op1)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, ($cType) op1)";
		}
		return $sprintf;
	}
	
	public function getResultSizePossibilities() {
		return "res_count + string_length";
	}

	public function getResultSizeCalculation() {
		$sprintf = $this->getPrintExpression();
		$lines = array();
		if($this->addressMode == "ARR") {
			$lines[] = "string_length = 2;";			
			$lines[] = "while(op1_ptr != op1_end) {";
			$lines[] = 		"string_length += $sprintf;";
			$lines[] =		"op1_ptr++;";
			$lines[] =		"if(op1_ptr != op1_end) {";
			$lines[] =			"string_length += 2;";
			$lines[] =		"}";
			$lines[] = "}";
		} else {
			$lines[] = "string_length = $sprintf;";
		}		 
		return $lines;
	}
	
	public function getAction() {
		$sprintf = $this->getPrintExpression();
		$lines = array();
		if($this->addressMode == "ARR") {
			$lines[] = "op1_ptr = op1_start;";
			$lines[] = "res_ptr += res_count_before;";
			$lines[] = "*res_ptr = '[';";
			$lines[] = "res_ptr++;";
			$lines[] = "while(op1_ptr != op1_end) {";
			$lines[] = 		"string_length = $sprintf;";
			$lines[] = 		"memcpy(res_ptr, sprintf_buffer, string_length);";
			$lines[] =		"res_ptr += string_length;";
			$lines[] =		"op1_ptr++;";
			$lines[] =		"if(op1_ptr != op1_end) {";
			$lines[] = 			"*res_ptr = ',';";
			$lines[] = 			"res_ptr++;";
			$lines[] = 			"*res_ptr = ' ';";
			$lines[] = 			"res_ptr++;";
			$lines[] =		"}";
			$lines[] = "}";
			$lines[] = "*res_ptr = ']';";
		} else {
			if($this->flags & self::SEARCHING_FOR_OPERANDS) {
				// since sprintf() is called in getResultSizeCalculation(), we don't need to do it again
				// during the initial operand scan though, getResultSizeCalculation() isn't invoked
				// need to add the line so the operands are picked up
				$lines[] = "// string_length = $sprintf;";
			}
			$lines[] = "memcpy(res_ptr + res_count_before, sprintf_buffer, string_length);";
		}
		return $lines;
	}
}

?>