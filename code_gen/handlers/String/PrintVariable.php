<?php

class PrintVariable extends Handler {

	use MultipleAddressMode, UseSprintf;
	
	public function getInputOperandCount() {
		return 1;
	}
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("op1");
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf;";
		$lines[] = "qb_write_output(cxt, sprintf_buffer, len);";
		return $lines;
	}

	public function getActionOnMultipleData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "qb_write_output(cxt, \"[\", 1);";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] =		"char sprintf_buffer[64];";
		$lines[] =		"uint32_t len = $sprintf;";
		$lines[] =		"qb_write_output(cxt, sprintf_buffer, len);";
		$lines[] = 		"op1_ptr++;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"qb_write_output(cxt, \", \", 2);";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "qb_write_output(cxt, \"]\", 1);";
		return $lines;
	}
}

?>