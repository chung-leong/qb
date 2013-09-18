<?php

class Clear extends Handler {

	public function getInputOperandCount() {
		return 0;
	}
	
	protected function getOperandDeclaration($i) {
		$addressMode = $this->getOperandAddressMode($i);
		if($addressMode == "ELE" || $addressMode == "ARR") {
			// don't need any thing
			return null;
		} else {
			return parent::getOperandDeclaration($i);
		}
	}
	
	protected function getOperandRetrievalCode($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		if($addressMode == "ELE") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = (($instr *) instruction_pointer)->operand{$i} >> 8;";
			$lines[] = "index = ((uint32_t *) segment0)[index_index];";
			
			// shrink the segment at index
			$lines[] = "if(index < segment_element_counts[selector]) {";
			$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], index, 1);";
			$lines[] = "}";
		} else if($addressMode == "ARR") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_index = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x03FF;";
			$lines[] = "size_index = (($instr *) instruction_pointer)->operand{$i} >> 20;";

			$lines[] = "res_start_index = ((uint32_t *) segment0)[index_index];";
			$lines[] = "res_count = ((uint32_t *) segment0)[size_index];";				
			$lines[] = "if(res_start_index + res_count <= segment_element_counts[selector] && res_start_index + res_count >= res_start_index) {";
			$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], res_start_index, res_count);";
			$lines[] = "}";
		} else {
			return parent::getOperandRetrievalCode($i);
		}
		$lines[] = "";
		return $lines;
	}
	
	public function getAction() {
		if($this->addressMode == "SCA") {
			return "res = 0;";
		} else {
			// nothing needs to be done
		}
	}
}

?>