<?php

class Matrix extends Handler {

	protected $operandPadding = 0;
	protected $order;

	public function __construct($name, $operandType, $vectorSize, $order = null, $multipleData = false) {
	}
	
	public function getName() {
		$name = parent::getName();
		if($this->operandPadding) {
			$name = str_replace("{$this->operandSize}X", "{$this->operandSize}XP", $name);
		}
		return $name;
	}
	
	public function getFunctionName() {
		$className = get_class($this);
		$opName = substr($className, 2, -7);
		$opName = preg_replace("/([a-z])([A-Z])/", "$1_$2", $opName);
		$opName = strtolower($opName);
		$name = "qb_do_{$opName}";
		if($this->order == "column-major") {
			$name .= "_cm";
		} else if($this->order == "row-major") {
			$name .= "_rm";
		}
		if($this->operandPadding > 0) {
			$name .= "_{$this->operandSize}x_padded";
		} else {
			if($this->operandSize != 1 && is_int($this->operandSize)) {
				$name .= "_{$this->operandSize}x";
			}
		}
		if($this->isMultipleData()) {
			$name .= "_multiple_times";
		}
		if($this->operandType) {
			$name .= "_{$this->operandType}";
		}
		return $name;
	}
	
	public function getfunctionType() {
		if(substr($this->operandType, 1) == 64) {
			// if it's double precision, overhead from function call probably doesn't matter
			return "extern";
		} else {
			return parent::getFunctionType();
		}
	}
	
	public function needsUnrolling() {
		return false;
	}
	
	public function needsMatrixDimensions($which = null) {
		if($this->operandSize == "variable") {
			if(!$which || $which <= $this->getInputOperandCount()) {
				return true;
			}
		} 
		return false;
	}
}