--TEST--
Face-forward test
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
	$b = array(1, 2, -3);
	$c = array(10, 5);
	$d = array(-1, -1);
	$e = array(array(1, 1), array(2, 3), array(0, -4), array(5, 0));
	$f = array(1, -1);
	
	echo faceforward($a, $b), "\n";
	echo faceforward($c, $d), "\n";
	echo faceforward($e, $f), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[-0.5, -0.5, -0.5]
[-10, -5]
[[-1, -1], [-2, -3], [0, -4], [5, 0]]
