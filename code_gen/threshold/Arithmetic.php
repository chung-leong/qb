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
		$a += 8;
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
		$a /= 8;
		$e = time();
		return ($e - $s);
	}
}

?>
