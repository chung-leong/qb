<?php

class Jump extends Handler {

	use NoOperands, JumpInstruction;

	public function getAction() {
		return null;
	}
}

?>