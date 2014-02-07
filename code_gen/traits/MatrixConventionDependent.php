<?php

trait MatrixConventionDependent {

	public function getMatrixConvention() {
		if(strpos($this->baseName, "_CM")) {
			return "column-major";
		} else {
			return "row-major";
		}
	}
	
	public function getPadding() {
		if(strpos($this->baseName, "_PAD")) {
			return 1;
		} else {
			return 0;
		}
	}
	
	protected function getFunctionNameComponents($prefix) {
		$parts = parent::getFunctionNameComponents($prefix);
		if($this->getPadding()) {
			array_splice($parts, -2, 0, 'padded');
		} else {
			array_splice($parts, -1, 0, str_replace('-', '_', $this->getMatrixConvention()));
		}
		return $parts;
	}
}

?>