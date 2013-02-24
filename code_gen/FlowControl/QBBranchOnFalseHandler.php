<?php

class QBBranchOnFalseHandler extends QBBranchHandler {

	public function getAction() {
		return "condition = !op1;";
	}
}

?>