<?php

class QBUnsetHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
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