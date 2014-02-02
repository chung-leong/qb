<?php

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

?>
