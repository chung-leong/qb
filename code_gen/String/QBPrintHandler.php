<?php

class QBPrintHandler extends QBHandler {

	protected function getSprintf() {
		$type = $this->getOperandType(1);
		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, op1)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, op1)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, op1)";
		}
	}	
}

?>