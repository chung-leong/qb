<?php

class QBReverseSortHandler extends QBSortHandler {

	public function __construct($baseName, $operandType = NULL, $addressMode = NULL, $vectorWidth = null) {
		parent::__construct($baseName, $operandType, $addressMode, $vectorWidth);
		$this->direction = "descending";
	}
}

?>