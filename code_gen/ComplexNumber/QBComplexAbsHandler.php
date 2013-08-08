<?php

class QBComplexAbsHandler extends QBComplexNumberHandler {

	public function getOperandSize($i) {
		return ($i == 2) ? "1" : "2";
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "expr_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			return "expr_count = op1_count / 2;";
		}
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		return "res_ptr[0] = ($cType) sqrt(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);";
	}
}

?>