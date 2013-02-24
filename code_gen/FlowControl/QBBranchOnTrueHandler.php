<?php

class QBBranchOnTrueHandler extends QBBranchHandler {

	public function getAction() {
		return "condition = op1;";
	}
}

?>