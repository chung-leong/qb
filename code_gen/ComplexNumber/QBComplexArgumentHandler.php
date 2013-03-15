<?php

class QBComplexArgumentHandler extends QBComplexNumberHandler {

	public function getOperandSize($i) {
		return ($i == 2) ? "1" : "2";
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "vector_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			return "vector_count = op1_count / 2;";
		}
	}

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return "res_ptr[0] = ($cType) atan2(op1_ptr[1], op1_ptr[0]);";
	}
}

?>