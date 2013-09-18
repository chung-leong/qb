<?php

class Jump extends Handler {

	use JumpInstruction;

	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$instr = $this->getInstructionStructure();
		$opCount = $this->getOperandCount();
		$lines[] = $this->getLabelCode($name);
		$lines[] = $this->getSetHandlerCode("(($instr *) instruction_pointer)->next_handler");
		$lines[] = "{";
		// retrieve operands (none by default)
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandDeclaration($i);
		}
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		// check time limit (Windows only)
		$lines[] = $this->getTimeoutCode();
		// in case a descendant class wants to do something before the jump occurs		
		$lines[] = $this->getAction();
		$lines[] = "}";
		$lines[] = "instruction_pointer = (($instr *) instruction_pointer)->instruction_pointer;";
		$lines[] = $this->getJumpCode();
		return $lines;
	}
	
	public function getAction() {
		// nothing happens by default
	}
}

?>