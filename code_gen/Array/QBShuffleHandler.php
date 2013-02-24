<?php

class QBShuffleHandler extends QBHandler {

	public function getAction() {
		return "qb_shuffle_{$this->operandType}(res, res_count);";
	}
}

?>