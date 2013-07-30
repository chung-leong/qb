<?php

class QBShuffleHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}

	public function getAction() {
		return "qb_shuffle_{$this->operandType}(res, res_count);";
	}
}

?>