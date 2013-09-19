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
	
	public function getFunctionName($prefix) {
		$name = parent::getFunctionName($prefix);
		if($this->getPadding()) {
			$name = preg_replace('/(\dx)/', '$1_padded', $name);
		}
		return $name;
	}
}

?>