<?php

class ConcatString extends Handler {

	use ArrayAddressMode, BinaryOperator;
	
	public function getActionOnUnitData() {
		return "memcpy(res_ptr + op2, op1_ptr, op1_count);";
	}
}

?>