<?php

class QBRandomMTHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"int64_t ZEND_FASTCALL qb_mt_rand_S64(qb_interpreter_context *cxt, int64_t lower_limit, uint64_t upper_limit) {",
					"USE_TSRM",
					"if(upper_limit > INT32_MAX || lower_limit < INT32_MIN) {",
						"int32_t number_h = php_mt_rand(TSRMLS_C) >> 1, upper_limit_h = upper_limit >> 32, lower_limit_h = lower_limit >> 32;",
						"uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = 0xFFFFFFFF, lower_limit_l = 0;",
						"number_h = lower_limit_h + (int32_t) ((double) ( (double) upper_limit_h - lower_limit_h + 1.0) * (number_h / (PHP_MT_RAND_MAX + 1.0)));",
						"number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));",
						"return (uint64_t) number_h << 32 | number_l;",
					"} else {",
						"int32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = (int32_t) upper_limit, lower_limit_l = (int32_t) lower_limit;",
						"number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));",
						"return number_l;",
					"}",
				"}",
			),
			array(
				"uint64_t ZEND_FASTCALL qb_mt_rand_U64(qb_interpreter_context *cxt, uint64_t lower_limit, uint64_t upper_limit) {",
					"USE_TSRM",
					"if(upper_limit > UINT32_MAX) {",
						"uint32_t number_h = php_mt_rand(TSRMLS_C) >> 1, upper_limit_h = upper_limit >> 32, lower_limit_h = lower_limit >> 32;",
						"uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = 0xFFFFFFFF, lower_limit_l = 0;",
						"number_h = lower_limit_h + (uint32_t) ((double) ( (double) upper_limit_h - lower_limit_h + 1.0) * (number_h / (PHP_MT_RAND_MAX + 1.0)));",
						"number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));",
						"return (uint64_t) number_h << 32 | number_l;",
					"} else {",
						"uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = (uint32_t) upper_limit, lower_limit_l = (uint32_t) lower_limit;",
						"number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));",
						"return number_l;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	protected function getScalarExpression() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		if($type[0] == 'U') {
			return "res = ($cType) qb_mt_rand_U64(cxt, op1, op2);";
		} else {
			return "res = ($cType) qb_mt_rand_S64(cxt, op1, op2);";
		}
	}
}

?>