<?php

class QBFunctionCallHandler extends QBHandler {

	public function __construct($name, $addressMode) {
		parent::__construct($name, NULL, $addressMode);
	}
	
	public function getCode() {
		$lines = array();
		$instr = $this->getInstructionStructure();
		$lines[] = $this->getDefineCode();
		$lines[] = $this->getLabelCode();
		$lines[] = $this->getSetHandlerCode("INSTRUCTION->next_handler");
		$lines[] = "{";
		$lines[] = 		"uint32_t symbol_index = INSTRUCTION->symbol_index;";
		$lines[] =		"uint32_t argument_count = INSTRUCTION->argument_count;";
		$lines[] = 		"uint32_t *operands = INSTRUCTION->operands, *op;";
		$lines[] = 		"uint32_t i;";
		$lines[] =		"zend_function *function = cxt->function->external_symbols[symbol_index]->pointer;";
		$lines[] =		"qb_initialize_function_call(cxt, function, argument_count, PHP_LINE_NUMBER);";
		if($this->addressMode == "VAR") {
			$lines[] =		"cxt->argument_address = &cxt->value_address;";
			$lines[] =		"cxt->value_address.segment_selector = QB_SELECTOR_VARIABLE;";
			$decodeFunction = "qb_decode_fcall_variable_operand";
		} else {
			$decodeFunction = "qb_decode_fcall_mix_operand";
		}
		$lines[] = 		"for(i = 0, op = operands; i < argument_count; i++) {";
		$lines[] =			"op += $decodeFunction(cxt, segments, op);";
		$lines[] =			"qb_copy_argument(cxt, i);";
		$lines[] =		"}";
		$lines[] = 		"qb_execute_function_call(cxt);";
		$lines[] = 		"for(i = 0, op = operands; i < argument_count + 1; i++) {";
		$lines[] =			"op += $decodeFunction(cxt, segments, op);";
		$lines[] = 			"qb_resync_argument(cxt, i);";
		$lines[] = 		"}";
		$lines[] =		"qb_finalize_function_call(cxt);";
		$lines[] =		"if(cxt->exception_encountered) {";
		$lines[] =			"goto label_exit;";
		$lines[] =		"}";
		$lines[] =		"instruction_pointer += sizeof(INSTRUCTION->next_handler) + INSTRUCTION->operand_size;";
		$lines[] = "}";
		$lines[] = $this->getJumpCode();
		$lines[] = $this->getUndefCode();
		return $lines;
	}
	
	public function getFunctionsUsed() {
		return array(
			"qb_initialize_function_call",
			"qb_copy_argument",
			"qb_execute_function_call",
			"qb_finalize_function_call",
			"qb_resync_argument",
		);
	}
		
	public function getOpFlags() {
		$flags = parent::getOpFlags();
		$flags[] = "QB_OP_VARIABLE_LENGTH";
		return $flags;
	}

	public function getHelperFunctions() {
		if($this->addressMode == "VAR") {
			$functions = array(
				array(
					"static uint32_t zend_always_inline qb_decode_fcall_variable_operand(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands) {",
						"uint32_t operand1 = operands[0];",
						"uint32_t var_operand2 = operands[1];",
						"uint32_t type = operand1 & 0x00FF;",
						"uint32_t flags = operand1 >> 16;",
						"uint32_t index = var_operand2;",
						"cxt->value_address.type = type;",
						"cxt->value_address.flags = flags;",
						"cxt->value_address.segment_offset = index << type_size_shifts[type];",
						"return 2;",
					"}",
				),
			);
			return $functions;
		} else {
			$functions = array(
				array(
					"static uint32_t ZEND_FASTCALL qb_decode_fcall_mix_operand(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands) {",
						"uint32_t operand_count = 0;",
						"uint32_t operand1 = operands[operand_count++];",
				 		"uint32_t type = operand1 & 0x00FF;",
				 		"uint32_t dimension = (operand1 >> 8) & 0x00FF;",
				 		"uint32_t flags = operand1 >> 16;",
						"if(dimension > 0) {",
							"uint32_t vec_operand2 = operands[operand_count++];",
				 			"uint32_t selector = vec_operand2 & 0x00FF;",
				 			"uint32_t index_index = (vec_operand2 >> 8) & 0x03FF;",
				 			"uint32_t array_start_index = ((uint32_t *) segments[0])[index_index];",
				 			"uint32_t j;",
							"cxt->argument_address = &cxt->array_address;",
				 			"cxt->array_address.segment_selector = selector;",
				 			"cxt->array_address.segment_offset = array_start_index << type_size_shifts[type];",
				 			"cxt->array_address.type = type;",
				 			"cxt->array_address.flags = flags;",
				 			"cxt->array_address.dimension_count = dimension;",
				 			"for(j = 0; j < dimension; j++) {",
				 				"// the size and dimension always share the same set of flags",
				 				"uint32_t flags = operands[operand_count++];",
				 				"uint32_t dimension_index = operands[operand_count++];",
				 				"uint32_t size_index = operands[operand_count++];",
				 				"cxt->array_address.dimension_addresses[j]->flags = flags >> 16;",
				 				"cxt->array_address.dimension_addresses[j]->segment_offset = dimension_index * sizeof(uint32_t);",
				 				"cxt->array_address.array_size_addresses[j]->flags = flags >> 16;",
				 				"cxt->array_address.array_size_addresses[j]->segment_offset = size_index * sizeof(uint32_t);",
				 			"}",
				 		"} else {",
							"uint32_t elv_operand2 = operands[operand_count++];",
				 			"uint32_t selector = elv_operand2 & 0x00FF;",
				 			"uint32_t index_selector = (elv_operand2 >> 8) & 0x00FF;",
				 			"uint32_t index_index = elv_operand2 >> 16;",
				 			"uint32_t index = ((uint32_t *) segments[index_selector])[index_index];",
							"cxt->argument_address = &cxt->value_address;",
				 			"cxt->value_address.type = type;",
				 			"cxt->value_address.flags = flags;",
				 			"cxt->value_address.segment_selector = selector;",
				 			"cxt->value_address.segment_offset = index << type_size_shifts[type];",
				 		"}",
				 		"return operand_count;",
				 	"}",
				),
			);
			return $functions;
		} 
	}
	
	public function getInstructionStructure() {
		$instr = "qb_instruction_fcall";
		if(!isset(self::$typeDecls[$instr])) {
			$lines = array();
			$lines[] = "void *next_handler;";
			$lines[] = "uint16_t operand_size;";
			$lines[] = "uint16_t argument_count;";
			$lines[] = "uint32_t line_number;";
			$lines[] = "uint32_t symbol_index;";
			$lines[] = "uint32_t operands[1];";
			self::$typeDecls[$instr] = array(
					"typedef struct $instr {",
						$lines,
					"} $instr;"
			);
		}
		return $instr;
	}
}

?>