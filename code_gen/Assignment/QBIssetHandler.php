<?php

class QBIssetHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return ($i == 2) ? "VAR" : $this->addressMode;
	}

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getAction() {
		if($this->addressMode == "VAR") {
			return "res = (op1 != 0);";
		} else {
			return "res = (op1_ptr) && (op1 != 0);";
		}
	}
	
	protected function getOperandRetrievalCode($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		$lines = array();
		if($addressMode == "ELV") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = (($instr *) instruction_pointer)->operand{$i} >> 8;";
			$lines[] = "index = ((uint32_t *) segment0)[index_index];";
			
			// set pointer to null
			$lines[] = "if(index >= segment_element_counts[selector]) {";
			$lines[] = 		"op{$i}_ptr = NULL;";
			$lines[] = "} else {";
			$lines[] = 		"op{$i}_ptr = (($cType *) segments[selector]) + index;";				
			$lines[] = "}";
		} else if($addressMode == "ARR") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x03FF;";
			$lines[] = "size_index = (($instr *) instruction_pointer)->operand{$i} >> 20;";
			
			$lines[] = "op{$i}_start_index = ((uint32_t *) segment0)[index_index];";
			$lines[] = "op{$i}_count = ((uint32_t *) segment0)[size_index];";
			$lines[] = "if(op{$i}_start_index + op{$i}_count > segment_element_counts[selector]) {";
			$lines[] = 		"op{$i}_ptr = NULL;";
			$lines[] = "} else {";
			$lines[] = 		"op{$i}_ptr = (($cType *) segments[selector]) + op{$i}_start_index;";
			$lines[] = "}";
		} else {
			return parent::getOperandRetrievalCode($i);
		}
		$lines[] = "";
		return $lines;
	}
}

?>