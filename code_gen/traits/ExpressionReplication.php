<?php

trait ExpressionReplication {

	public function needsReplication() {
		if($this->operandSize > 1) {
			return true;
		}
	}
}

?>