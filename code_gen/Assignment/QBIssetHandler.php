<?php

class QBIssetHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return ($i == 2) ? "VAR" : $this->addressMode;
	}

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getAction() {
		$addressMode = $this->getOperandAddressMode(1);
		if($addressMode == "VAR") {
			return "res = (op1 != 0);";
		} else if($addressMode == "ELV") {
			return "res = (op1_ptr) && (op1 != 0);";
		} else if($addressMode == "ARR") {
			return "res = (op1_count > 0);";
		}
	}
	
	protected function getOperandRetrievalCode() {
		$cType = $this->getOperandCType(1);
		$addressMode = $this->getOperandAddressMode(1);
		$lines = array();
		$lines[] = $this->getOperandUnpackCode(1, $addressMode);
		if($addressMode == "VAR") {
			$lines[] = "op1_ptr = (($cType *) segment0) + index;";
		} else if($addressMode == "ELV") {
			// set pointer to null
			$lines[] = "if(index >= segment_element_counts[selector]) {";
			$lines[] = 		"op1_ptr = NULL;";
			$lines[] = "} else {";
			$lines[] = 		"op1_ptr = (($cType *) segments[selector]) + index;";				
			$lines[] = "}";
		} else if($addressMode == "ARR") {
			$lines[] = "index = ((uint32_t *) segment0)[index_index];";
			$lines[] = "op1_count = ((uint32_t *) segment0)[size_index];";
			$lines[] = "if(index + op1_count > segment_element_counts[selector] || index + op1_count < index) {";
			$lines[] = 		"op1_ptr = NULL;";
			$lines[] = "} else {";
			$lines[] = 		"op1_ptr = (($cType *) segments[selector]) + index;";
			$lines[] = "}";
		}
		$lines[] = "";
		
		$cType = $this->getOperandCType(2);
		$addressMode = $this->getOperandAddressMode(2);
		$lines[] = $this->getOperandUnpackCode(2, "VAR");
		$lines[] = "res_ptr = (($cType *) segment0) + index;";
		$lines[] = "";		
		return $lines;
	}
}

?>