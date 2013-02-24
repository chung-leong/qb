<?php

class QBLCGValueHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) php_combined_lcg(TSRMLS_C);";
	}
}

?>