<?php

abstract class QBBranchHandler extends QBHandler {

	public function getInstructionStructure() {
		if($this->flags & (self::SEARCHING_FOR_OPERANDS | self::SEARCHING_FOR_LINE_NUMBER)) {
			return 'INSTRUCTION_STRUCTURE';
		}
		$instr = "qb_instruction_branch_$this->opCount";
		if($this->flags & self::NEED_LINE_NUMBER) {
			$instr .= '_lineno';
		}
		if(!isset(self::$typeDecls[$instr])) {
			$lines = array();
			$lines[] = "void *next_handler1;";
			$lines[] = "int8_t *instruction_pointer1;";
			$lines[] = "void *next_handler2;";
			$lines[] = "int8_t *instruction_pointer2;";
			for($i = 1; $i <= $this->opCount; $i++) {
				$lines[] = "uint32_t operand$i;";
			}
			if($this->flags & self::NEED_LINE_NUMBER) {
				$lines[] = "uint32_t line_number;";
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
		array_splice($flags, 0, 0, array("QB_OPERAND_JUMP_TARGET", "QB_OPERAND_JUMP_TARGET"));
		return $flags;
	}

	public function getCode() {
		$lines = array();
		$lines[] = $this->getDefineCode();
		$lines[] = $this->getLabelCode();
		$lines[] = $this->getSetHandlerCode("INSTRUCTION->next_handler1");
		$lines[] = "{";
		for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] =	$this->getOperandRetrievalCode($i);
		}
		$lines[] =		$this->getTimeoutCode();
		// code returned by getAction() sets the variable condition
		$lines[] = 		$this->getAction();
		$lines[] = 		"if(condition) {";
		$lines[] = 			"instruction_pointer = INSTRUCTION->instruction_pointer1;";
		$lines[] = 		"} else {";
		$lines[] = 			$this->getSetHandlerCode("INSTRUCTION->next_handler2");
		$lines[] = 			"instruction_pointer = INSTRUCTION->instruction_pointer2;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = $this->getJumpCode();
		$lines[] = $this->getUndefCode();
		return $lines;
	}
	
	protected function getArrayExpression() {
		$name = $this->getName();
		throw new Exception("No implemenation for employing $name in array operation\n");
	}
	
	protected function initialize() {
		$this->flags |= self::WILL_JUMP;
	}
}

?>