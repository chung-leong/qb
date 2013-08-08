<?php

abstract class QBBranchHandler extends QBHandler {

	public function getJumpTargetCount() {
		return 2;
	}
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function getOperandFlags() {
		$flags = parent::getOperandFlags();
		array_splice($flags, 0, 0, array("QB_OPERAND_JUMP_TARGET", "QB_OPERAND_JUMP_TARGET"));
		return $flags;
	}
	
	public function getOperandDeclarations() {
		$lines = parent::getOperandDeclarations();
		$lines[] = "int32_t condition;";
		return $lines;
	}

	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$instr = $this->getInstructionStructure();
		$opCount = $this->getOperandCount();
		$lines[] = $this->getLabelCode($name);
		$lines[] = $this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler1");
		$lines[] = "{";
		if($this->needsLineNumber()) {
			$lines[] = "#define PHP_LINE_NUMBER	(($instr *) instruction_pointer)->line_number";
		}
		$lines[] =		$this->getOperandDeclarations();
		$lines[] =		$this->getOperandRetrievalCode();
		$lines[] =		$this->getTimeoutCode();
		// code returned by getAction() sets the variable condition
		$lines[] = 		$this->getAction();
		$lines[] = 		"if(condition) {";
		$lines[] = 			"instruction_pointer = (($instr *) instruction_pointer)->instruction_pointer1;";
		$lines[] = 		"} else {";
		$lines[] = 			$this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler2");
		$lines[] = 			"instruction_pointer = (($instr *) instruction_pointer)->instruction_pointer2;";
		$lines[] = 		"}";
		if($this->needsLineNumber()) {
			$lines[] = "#undef PHP_LINE_NUMBER";
		}
		$lines[] = "}";
		$lines[] = $this->getJumpCode();
		return $lines;
	}
	
	protected function getActionForMultipleData() {
		$name = $this->getName();
		throw new Exception("No implemenation for employing $name in array operation\n");
	}
}

?>