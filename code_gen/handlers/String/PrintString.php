<?php

class PrintString extends Handler {

	use ArrayAddressMode;
	
	public function getInputOperandCount() {
		return 1;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getAction() {
		return "qb_write_output(cxt, op1_ptr, op1_count);";
	}
}

?>