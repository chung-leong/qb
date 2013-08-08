<?php

class QBUnsetHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}
	
	protected function getOperandDeclarations() {
		$cType = $this->getOperandCType(1);
		$addressMode = $this->getOperandAddressMode(1);
		$lines = array();
		$lines[] = "uint32_t index;";
		if($addressMode == "VAR") {
			$lines[] = "$cType *__restrict res_ptr;";
		} else if($addressMode == "ELV" || $addressMode == "ARR") {
			$lines[] = "uint32_t selector;";
			$lines[] = "uint32_t index_index;";
			if($addressMode == "ARR") {
				$lines[] = "uint32_t size_index;";
				$lines[] = "uint32_t res_count;";
			}
		}
		return $lines;
	}
	
	protected function getOperandRetrievalCode() {
		$cType = $this->getOperandCType(1);
		$addressMode = $this->getOperandAddressMode(1);
		$lines = array();
		$lines[] = $this->getOperandUnpackCode(1, $addressMode);
		
		if($addressMode == "VAR") {
			$lines[] = "res_ptr = (($cType *) segment0) + index;";
		} else if($addressMode == "ELV") {
			// shrink the segment at index
			$lines[] = "if(index < segment_element_counts[selector]) {";
			$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], index, 1);";
			$lines[] = "}";
		} else if($addressMode == "ARR") {
			$lines[] = "index = ((uint32_t *) segment0)[index_index];";
			$lines[] = "res_count = ((uint32_t *) segment0)[size_index];";				
			$lines[] = "if(index + res_count <= segment_element_counts[selector] && index + res_count >= index) {";
			$lines[] = 		"qb_shrink_segment(cxt, &cxt->storage->segments[selector], index, res_count);";
			$lines[] = "}";
		}
		$lines[] = "";
		return $lines;
	}
	
	public function getAction() {
		if($this->addressMode == "VAR") {
			return "res = 0;";
		} else {
			// nothing needs to be done
			return null;
		}
	}
}

?>