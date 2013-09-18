<?php

trait BranchInstruction {

	public function getOutputOperandCount() {
		return 0;
	}
	
	public function getJumpTargetCount() {
		return 2;
	}
}

?>