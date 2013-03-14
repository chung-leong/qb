<?php

class QBComplexNumberHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getOperandSize($i) {
		return "2";
	}
	
	protected function expandComplexOperands($s) {
		if(is_array($s)) {
			$r = array();
			foreach($s as $a) {
				$r[] = $this->expandComplexOperands($a);
			}
			return $r;
		} else {
			return strtr($s, array(
				'op1_r' => 'op1_ptr[0]',
				'op1_i' => 'op1_ptr[1]',
				'op2_r' => 'op1_ptr[0]',
				'op2_i' => 'op1_ptr[1]',
				'res_r' => 'res_ptr[0]',
				'res_i' => 'res_ptr[1]',
			));
		}
	}

	public function getAction() {
		$expr = $this->getComplexNumberExpression();
		$expr = $this->expandComplexOperands($expr);
		return ($this->addressMode == "ARR") ? $this->getIterationCode($expr) : $expr;
	}	
}

?>