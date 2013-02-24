<?php

class QBIssetHandler extends QBHandler {

	protected function initialize() {
		$this->flags |= self::IS_ISSET;
	}
	
	public function getOperandAddressMode($i) {
		return ($i == 2) ? "VAR" : $this->addressMode;
	}

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getAction() {
		if($this->addressMode == "VAR") {
			return "res = (op1 != 0);";
		} else {
			return "res = (op1_ptr) && (op1 != 0);";
		}
	}
}

?>