<?php

class BranchOnNotEqual extends Handler {

	use MultipleAddressMode, BinaryOperatorNoResult, BranchInstruction, NoUnsigned;
	
	protected function getActionOnUnitData() {
		return "condition = (op1 != op2);";
	}
}

?>