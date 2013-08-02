<?php

class QBReflectHandler extends QBMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$type = $this->getOperandType(3);
		if($this->operandSize == "variable") {
			$lines[] = "uint32_t i;";
			$lines[] = "$cType dot_product;";
			$lines[] = "qb_calculate_dot_product_$type(op1_ptr, NULL, op2_ptr, NULL, MATRIX2_ROWS, &dot_product, NULL);";
			$lines[] = "for(i = 0; i < MATRIX2_ROWS; i++) {";
			$lines[] = 		"res_ptr[i] = ($cType) (op1_ptr[i] - 2.0 * dot_product * op2_ptr[i]);";
			$lines[] = "}";
		} else {
			switch($this->operandSize) {
				case 2: {
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_2x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
				}	break;
				case 3: {
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_3x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
					$lines[] = "res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);";
				}	break;
				case 4: {
					$lines[] = "$cType dot_product;";
					$lines[] = "qb_calculate_dot_product_4x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);";
					$lines[] = "res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);";
					$lines[] = "res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);";
					$lines[] = "res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);";
					$lines[] = "res_ptr[3] = ($cType) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);";
				}	break;
			}
		}
		return $lines;
	}
}

?>