--TEST--
Reflection test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3]		$a
 * @local	float32[3]		$b
 * @local	float32[2]		$c
 * @local	float64[2]		$d
 * @local	float64[][2]	$e
 * @local	float64[2]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5);
	$b = array(1, 2, 3);
	$c = array(10, 5);
	$d = array(-1, -1);
	$e = array(array(1, 1), array(2, 3), array(0, -4), array(5, 0));
	$f = array(1, -1);
	
	echo reflect($a, $b), "\n";
	echo reflect($c, $d), "\n";
	echo reflect($e, $f), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[-5.5, -11.5, -17.5]
[-20, -25]
[[1, 1], [4, 1], [-8, 4], [-5, 10]]
