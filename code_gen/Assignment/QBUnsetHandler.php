<?php

class QBUnsetHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}
	
	protected function getOperandRetrievalCode($i) {
		$instr = $this->getInstructionStructure();
		$cType = $this->getOperandCType($i);
		$addressMode = $this->getOperandAddressMode($i);
		if($addressMode == "ELV") {
			$lines[] = "selector = (($instr *) instruction_pointer)->operand{$i} & 0x00FF;";
			$lines[] = "index_selector = ((($instr *) instruction_pointer)->operand{$i} >> 8) & 0x00FF;";
			$lines[] = "index_index = (($instr *) instruction_pointer)->operand{$i} >> 16;";
			$lines[] = "index = ((uint32_t *) segments[index_selector])[index_index];";
			
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
		if($this->addressMode == "VAR") {
			return "res = 0;";
		} else {
			// nothing needs to be done
		}
	}
}

?>