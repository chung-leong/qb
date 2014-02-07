<?php

class ComplexAbs_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = cabs($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexArgument_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = carg($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexCos_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ccos($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexCosh_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ccosh($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexDivide_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 5;
		$s = time();
		$a = cdiv($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ComplexExp_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cexp($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexLog_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = clog($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexMultiply_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 5;
		$s = time();
		$a = cmult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ComplexPow_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cpow($a, array(2, 3));
		$e = time();
		return ($e - $s);
	}
}

class ComplexSin_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csin($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexSinh_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csinh($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexSquareRoot_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexTan_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ctan($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexTanh_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ctanh($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexAbs_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = cabs($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexArgument_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = carg($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexCos_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ccos($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexCosh_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ccosh($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexDivide_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 5;
		$s = time();
		$a = cdiv($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ComplexExp_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cexp($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexLog_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = clog($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexMultiply_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 5;
		$s = time();
		$a = cmult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ComplexPow_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cpow($a, array(2, 3));
		$e = time();
		return ($e - $s);
	}
}

class ComplexSin_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csin($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexSinh_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csinh($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexSquareRoot_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = csqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexTan_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ctan($a);
		$e = time();
		return ($e - $s);
	}
}

class ComplexTanh_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ctanh($a);
		$e = time();
		return ($e - $s);
	}
}

?>
