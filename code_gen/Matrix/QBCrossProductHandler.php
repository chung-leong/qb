<?php

class QBCrossProductHandler extends QBMatrixHandler {

	public function __construct($name, $operandType, $addressMode) {
		parent::__construct($name, $operandType, $addressMode, 3);
	}
	
	public function getInputOperandCount() {
		return 2;
	}

	public function getActionForUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		$lines[] = "$cType i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);";
		$lines[] = "$cType j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);";
		$lines[] = "$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);";
		$lines[] = "res_ptr[0] = i;";
		$lines[] = "res_ptr[1] = j;";
		$lines[] = "res_ptr[2] = k;";
		return $lines;
	}
}

?>