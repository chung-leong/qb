<?php

class RandomMT extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly, Slow;
	
	public function needsInterpreterContext() {
		return true;
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		$width = (int) substr($type, 1);
		$lines = array();
		if($type[0] == 'U') {
			if($width > 32) {
				$lines[] = "USE_TSRM";
				$lines[] = "uint32_t number_h = php_mt_rand(TSRMLS_C) >> 1, upper_limit_h = op2 >> 32, lower_limit_h = op1 >> 32;";
				$lines[] = "uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = 0xFFFFFFFF, lower_limit_l = 0;";
				$lines[] = "number_h = lower_limit_h + (uint32_t) ((double) ( (double) upper_limit_h - lower_limit_h + 1.0) * (number_h / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "res_ptr[0] = (uint64_t) number_h << 32 | number_l;";
			} else {
				$lines[] = "USE_TSRM";
				$lines[] = "uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = (uint32_t) op2, lower_limit_l = (uint32_t) op1;";
				$lines[] = "number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "res_ptr[0] = ($cType) number_l;";
			}
		} else {
			if($width > 32) {
				$lines[] = "USE_TSRM";
				$lines[] = "int32_t number_h = php_mt_rand(TSRMLS_C) >> 1, upper_limit_h = op2 >> 32, lower_limit_h = op1 >> 32;";
				$lines[] = "uint32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = 0xFFFFFFFF, lower_limit_l = 0;";
				$lines[] = "number_h = lower_limit_h + (int32_t) ((double) ( (double) upper_limit_h - lower_limit_h + 1.0) * (number_h / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "res_ptr[0] = (uint64_t) number_h << 32 | number_l;";
			} else {
				$lines[] = "USE_TSRM";
				$lines[] = "int32_t number_l = php_mt_rand(TSRMLS_C) >> 1, upper_limit_l = (int32_t) op2, lower_limit_l = (int32_t) op1;";
				$lines[] = "number_l = lower_limit_l + (uint32_t) ((double) ( (double) upper_limit_l - lower_limit_l + 1.0) * (number_l / (PHP_MT_RAND_MAX + 1.0)));";
				$lines[] = "res_ptr[0] = ($cType) number_l;";
			}
		}
		return $lines;
	}
}

?>