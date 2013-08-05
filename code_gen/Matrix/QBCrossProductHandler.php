<?php

class QBCrossProductHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1:
			case 2:	return $this->operandSize;
			case 3: {
				switch($this->operandSize) {
					case 2: return 1;
					case 3: return 3;
				}
			}
		}
	}

	public function needsMatrixDimensions($which = null) {
		if($this->operandSize == "variable") {
			if($which == 1) {
				// only need the first, since the second must have the same size
				return true;
			}
		}
		return false;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		switch($this->operandSize) {
			case 2: {
				$lines[] = "$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);";
				$lines[] = "res_ptr[0] = k;";
			}	break;
			case 3: {
				$lines[] = "$cType i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);";
				$lines[] = "$cType j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);";
				$lines[] = "$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);";
				$lines[] = "res_ptr[0] = i;";
				$lines[] = "res_ptr[1] = j;";
				$lines[] = "res_ptr[2] = k;";
			}	break;
		}
		return $lines;
	}
}

?>