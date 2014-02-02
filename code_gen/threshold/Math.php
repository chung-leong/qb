<?php

class Sqrt_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sqrt($a);
		$e = time();
		return ($e - $s);
	}
}

?>
