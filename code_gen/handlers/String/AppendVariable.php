<?php

class AppendVariable extends Handler {

	use MultipleAddressMode, BinaryOperator, UseSprintf, ResizeResult, NotThreadSafe {
        ResizeResult::needsInterpreterContext insteadof UseSprintf;
	}
	
	public function getOperandType($i) {
		list($srcType, $dstType) = explode('_', $this->operandType);
		switch($i) {
			case 1: return $srcType;
			case 2: return "U32";
			case 3: return $dstType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return "CON";
			case 3: return "ARR";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return "op3_count";
		}
	}
	
	public function getActionOnUnitData() {
		$dstCType = $this->getOperandCType(3);
		$sprintf = $this->getSprintf("op1");
		$lines = array();
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf, i;";
		$lines[] = "$dstCType *res_end;";
		$lines[] = "res_ptr = ($dstCType *) (((char *) res_ptr) + qb_resize_segment(&cxt->function->local_storage->segments[op2], sizeof($dstCType) * (res_count + len)));";
		$lines[] = "res_end = res_ptr + res_count;";
		$lines[] = "for(i = 0; i < len; i++) {";
		$lines[] =		"res_end[i] = sprintf_buffer[i];";
		$lines[] = "}";
		$lines[] = "res_count += len;";
		return $lines;	
	}
	
	public function getActionOnMultipleData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$srcCType = $this->getOperandCType(1);
		$dstCType = $this->getOperandCType(3);
		$lines = array();
		$lines[] = "uint32_t pos = res_count;";
		$lines[] = "uint32_t total = 0;";
		$lines[] = "$srcCType *op1_start = op1_ptr;";
		$lines[] = "$srcCType *op1_end = op1_ptr + op1_count;";
		$lines[] = "if(op1_count) {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] = 			"char sprintf_buffer[64];";
		$lines[] = 			"uint32_t len = $sprintf;";
		$lines[] = 			"total += len;";
		$lines[] = 			"op1_ptr++;";
		$lines[] = 		"}";
		$lines[] = 		"total += op1_count * 2;";
		$lines[] = "} else {";
		$lines[] = 		"total = 2;";
		$lines[] = "}";
		$lines[] = "res_ptr = ($dstCType *) (((char *) res_ptr) + qb_resize_segment(&cxt->function->local_storage->segments[op2], sizeof($dstCType) * (res_count + total)));";
		$lines[] = "res_ptr[pos++] = '[';";
		$lines[] = "op1_ptr = op1_start;";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] = 		"char sprintf_buffer[64];";
		$lines[] = 		"uint32_t len = $sprintf, i;";
		$lines[] = 		"$dstCType *res_end = res_ptr + pos;";
		$lines[] =		"for(i = 0; i < len; i++) {";
		$lines[] =			"res_end[i] = sprintf_buffer[i];";
		$lines[] =		"}";
		$lines[] = 		"pos += len;";
		$lines[] = 		"op1_ptr++;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"res_ptr[pos++] = ',';";
		$lines[] = 			"res_ptr[pos++] = ' ';";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res_ptr[pos++] = ']';";
		$lines[] = "res_count += total;";
		return $lines;
	}
}

?>