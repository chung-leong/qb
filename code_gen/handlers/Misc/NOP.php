<?php

class NOP extends Handler {

	use NoOperands;

	public function getAction() {
		return null;
	}
}

?>