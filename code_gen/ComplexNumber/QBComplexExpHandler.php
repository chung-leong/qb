<?php

class QBComplexExpHandler extends QBComplexNumberHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType w = exp$f(op1_ptr[0]);";
		$lines[] = "$cType r = w * cos$f(op1_ptr[1]);";
		$lines[] = "$cType i = w * sin$f(op1_ptr[1]);";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>