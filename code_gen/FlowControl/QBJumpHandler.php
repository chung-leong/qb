<?php

class QBJumpHandler extends QBHandler {

	public function getInstructionStructure() {
		$instr = "qb_instruction_jump_$this->opCount";
		if(!isset(self::$typeDecls[$instr])) {
			$lines = array();
			$lines[] = "void *next_handler;";
			$lines[] = "int8_t *instruction_pointer;";
			for($i = 1; $i <= $this->opCount; $i++) {
				$lines[] = "uint32_t operand$i;";
			}
			self::$typeDecls[$instr] = array(
					"typedef struct $instr {",
						$lines,
					"} $instr;"
			);
		}
		return $instr;
	}
	
	public function getOperandFlags() {
		$flags = parent::getOperandFlags();
		array_unshift($flags, "QB_OPERAND_JUMP_TARGET");
		return $flags;
	}

	public function getCode() {
		$lines = array();
		$lines[] = $this->getDefineCode();
		$lines[] = $this->getLabelCode();
		$lines[] = $this->getSetHandlerCode("INSTRUCTION->next_handler");
		$lines[] = "{";
		// check time limit (Windows only)
		$lines[] = $this->getTimeoutCode();
		// in case a descendant class wants to do something before the jump occurs		
		$lines[] = $this->getAction();
		$lines[] = "}";
		$lines[] = "instruction_pointer = INSTRUCTION->instruction_pointer;";
		$lines[] = $this->getJumpCode();
		$lines[] = $this->getUndefCode();
		return $lines;
	}
	
	public function getAction() {
		// nothing happens by default
	}
	
	protected function initialize() {
		$this->flags |= self::WILL_JUMP;
	}
}

?>