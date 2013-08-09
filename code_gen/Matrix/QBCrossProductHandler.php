<?php

class QBCrossProductHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		if($this->operandSize == 4) {
			return 3;
		} else {
			return 2;
		}
	}
	
	public function needsMatrixDimensions($which = null) {
		if($this->operandSize == "variable") {
			if(!$which || $which == 1) {
				// only need the first, since the second must have the same size
				return true;
			}
		}
		return false;
	}
	
	public function getFunctionType() {
		if($this->operandSize == 4) {
			// 4D vector cross-product is rarely used
			return "extern";
		} else {
			return parent::getFunctionType();
		}
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$lines = array();
		switch($this->operandSize) {
			case 2: {
				$lines[] = "$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);";
				$lines[] = "res_ptr[0] = k;";
				$lines[] = "res_ptr[1] = k;";
			}	break;
			case 3: {
				$lines[] = "$cType i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);";
				$lines[] = "$cType j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);";
				$lines[] = "$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);";
				$lines[] = "res_ptr[0] = i;";
				$lines[] = "res_ptr[1] = j;";
				$lines[] = "res_ptr[2] = k;";
			}	break;
			case 4: {
				$lines[] = "$cType i = op1_ptr[1] * ((op2_ptr[2] * op3_ptr[3]) - (op3_ptr[2] * op2_ptr[3])) - op1_ptr[2] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[1] * op3_ptr[2]) - (op3_ptr[1] * op2_ptr[2]));";
				$lines[] = "$cType j = op1_ptr[0] * ((op3_ptr[2] * op2_ptr[3]) - (op2_ptr[2] * op3_ptr[3])) - op1_ptr[2] * ((op3_ptr[0] * op2_ptr[3]) - (op2_ptr[0] * op3_ptr[3])) + op1_ptr[3] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2]));";
				$lines[] = "$cType k = op1_ptr[0] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) - op1_ptr[1] * ((op2_ptr[0] * op3_ptr[3]) - (op3_ptr[0] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[0] * op3_ptr[1]) - (op3_ptr[0] * op2_ptr[1]));";
				$lines[] = "$cType m = op1_ptr[0] * ((op3_ptr[1] * op2_ptr[2]) - (op2_ptr[1] * op3_ptr[2])) - op1_ptr[1] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2])) + op1_ptr[2] * ((op3_ptr[0] * op2_ptr[1]) - (op2_ptr[0] * op3_ptr[1]));";
				$lines[] = "res_ptr[0] = i;";
				$lines[] = "res_ptr[1] = j;";
				$lines[] = "res_ptr[2] = k;";
				$lines[] = "res_ptr[3] = m;";
			}	break;
		}
		return $lines;
	}
}

?>