<?php

class QBComplexSinhHandler extends QBComplexNumberHandler {

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType r = sinh$f(op1_ptr[0]) * cos$f(op1_ptr[1]);";
		$lines[] = "$cType i = cosh$f(op1_ptr[0]) * sin$f(op1_ptr[1]);";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>