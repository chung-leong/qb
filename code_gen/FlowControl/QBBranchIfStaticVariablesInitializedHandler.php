<?php

class QBBranchIfStaticVariablesInitializedHandler extends QBBranchHandler {

	public function getInputOperandCount() {
		return 0;
	}

	public function getAction() {
		$lines = array();
		$lines[] = "if(cxt->storage->flags & QB_STORAGE_STATIC_INITIALIZED) {";
		$lines[] =		"condition = 1;";
		$lines[] = "} else {";
		$lines[] = 		"condition = 0;";
		$lines[] = 		"cxt->storage->flags |= QB_STORAGE_STATIC_INITIALIZED;";
		$lines[] = "}";
		return $lines;
	}
}

?>