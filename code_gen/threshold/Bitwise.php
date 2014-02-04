<?php

class BitwiseAnd_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a &= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseNot_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ~$a;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseOr_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a |= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseXor_I08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a ^= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_S08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_S08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a >>= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_U08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_U08 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseAnd_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a &= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseNot_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ~$a;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseOr_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a |= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseXor_I16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a ^= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_S16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_S16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a >>= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_U16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_U16 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseAnd_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a &= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseNot_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ~$a;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseOr_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a |= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseXor_I32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a ^= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_S32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_S32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a >>= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_U32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_U32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseAnd_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a &= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseNot_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ~$a;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseOr_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a |= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class BitwiseXor_I64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a ^= 0x0001;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_S64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_S64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a >>= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftLeft_U64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

class ShiftRight_U64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param uint64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a <<= 2;
		$e = time();
		return ($e - $s);
	}
}

?>
