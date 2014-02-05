<?php

class Add_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Add_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Add_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class MultiplyAccumlate_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local int32[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 9;
		$s = time();
		$a += $b * M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Add_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class MultiplyAccumlate_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local int64[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 9;
		$s = time();
		$a += $b * M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Add_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class FlooredDivisionModulo_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = mod($a, M_PI);
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Modulo_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a %= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class MultiplyAccumlate_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float32[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 9;
		$s = time();
		$a += $b * M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Add_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a += M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Decrememt_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a--;
		$e = time();
		return ($e - $s);
	}
}

class Divide_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a /= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class FlooredDivisionModulo_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = mod($a, M_PI);
		$e = time();
		return ($e - $s);
	}
}

class Incrememt_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a++;
		$e = time();
		return ($e - $s);
	}
}

class Modulo_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a %= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Multiply_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a *= M_PI;
		$e = time();
		return ($e - $s);
	}
}

class MultiplyAccumlate_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 9;
		$s = time();
		$a += $b * M_PI;
		$e = time();
		return ($e - $s);
	}
}

class Negate_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

class Subtract_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = -$a;
		$e = time();
		return ($e - $s);
	}
}

?>
