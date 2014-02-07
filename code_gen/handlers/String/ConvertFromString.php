<?php

class ConvertFromString extends Handler {

	use MultipleAddressMode, UnaryOperator, Slow;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U08";
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
		}
	}

	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "ALLOCA_FLAG(use_heap)";
		$lines[] = "char *buffer = do_alloca(op1_count + 1, use_heap);";
		$lines[] = "memcpy(buffer, op1_ptr, op1_count);";
		$lines[] = "buffer[op1_count] = '\\0';";
		switch($this->operandType) {
			case 'S08':
				$lines[] = "res = (int8_t) strtol(buffer, NULL, 10);";
				break;
			case 'U08':
				$lines[] = "res = (uint8_t) strtoul(buffer, NULL, 10);";
				break;
			case 'S16':
				$lines[] = "res = (int16_t) strtol(buffer, NULL, 10);";
				break;
			case 'U16':
				$lines[] = "res = (uint16_t) strtoul(buffer, NULL, 10);";
				break;
			case 'S32':
				$lines[] = "res = strtol(buffer, NULL, 10);";
				break;
			case 'U32':
				$lines[] = "res = strtoul(buffer, NULL, 10);";
				break;
			case 'S64':
				$lines[] = "res = strtoll(buffer, NULL, 10);";
				break;
			case 'U64':
				$lines[] = "res = strtoull(buffer, NULL, 10);";
				break;
			case 'F32':
				$lines[] = "res = (float32_t) zend_strtod(buffer, NULL);";
				break;
			case 'F64':
				$lines[] = "res = zend_strtod(buffer, NULL);";
				break;
		}
		$lines[] = "free_alloca(buffer, use_heap);";
		return $lines;
	}
}

?>