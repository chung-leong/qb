<?php

class QBUnsetHandler extends QBHandler {

	protected function initialize() {
		if($this->addressMode == "ELC" || $this->addressMode == "ELV" || $this->addressMode == "ARR") {
			$this->flags |= self::IS_UNSET;
		}
	}
	
	public function getAction() {
		if($this->addressMode == "VAR") {
			return "res = 0;";
		} else {
			if($this->flags & self::SEARCHING_FOR_OPERANDS) {
				// nothing is done here but we still want the scan to pick up the variable used 
				return "// res_ptr ";
			}
		}
	}
}

?>