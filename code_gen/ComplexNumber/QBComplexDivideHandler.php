<?php

class QBComplexDivideHandler extends QBComplexNumberHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionForUnitData() {
		$type = $this->getOperandType(3);
		$cType = $this->getOperandCType(3);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType w = op2_ptr[0] * op2_ptr[0] + op2_ptr[1] * op2_ptr[1];";
		$lines[] = "$cType r = ((op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1])) / w;";
		$lines[] = "$cType i = ((op1_ptr[1] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[1])) / w;";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>