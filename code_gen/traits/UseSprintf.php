<?php

trait UseSprintf {

	public function needsInterpreterContext() {
		return true;
	}

	protected function getSprintf($operand) {
		$type = $this->getOperandType(1);
		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, $operand)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, $operand)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, $operand)";
		}
		return $sprintf;
	}	
	
	public function getFunctionType() {
		return 'extern';
	}
}

?>