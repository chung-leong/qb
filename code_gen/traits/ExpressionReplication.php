<?php

trait ExpressionReplication {

	public function needsUnrolling() {
		if($this->operandSize > 1) {
			return true;
		}
	}
}

?>