<?php

class QBInArrayHandler extends QBArraySearchHandler {
	
	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		$searchHandler = new QBArraySearchHandler(null, $this->operandType, $this->addressMode);
		$searchFunction = $searchHandler->getFunctionName();
		$lines = array();
		$lines[] = "int32_t index;";
		$lines[] = "$searchFunction(op1_ptr, op1_count, op2, &index);";
		$lines[] = "res = (index != -1);";
		return $lines;
	}
	
	protected function getActionOnMultipleData() {
		$searchHandler = new QBArraySearchHandler(null, $this->operandType, $this->addressMode);
		$searchFunction = $searchHandler->getFunctionName();
		$lines = array();
		$lines[] = "int32_t index;";
		$lines[] = "$searchFunction(op1_ptr, op1_count, op2_ptr, op2_count, &index);";
		$lines[] = "res = (index != -1);";
		return $lines;
	}
}

?>