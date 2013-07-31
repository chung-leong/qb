<?php

class QBComplexNumberHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getOperandSize($i) {
		return "2";
	}
}

?>