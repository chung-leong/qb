<?php

class QBAbsHandler extends QBHandler {

	protected function getScalarExpression() {
		$functions = array(
			"S08" => "abs",
			"S16" => "abs",
			"S32" => "abs",
			"S64" => "llabs",
			"F32" => "fabs",
			"F64" => "fabs",
		);
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$function = $functions[$type];
		return "res = ($cType) $function(op1);";
	}
}

?>