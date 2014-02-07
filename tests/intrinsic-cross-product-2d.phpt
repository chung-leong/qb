--TEST--
Cross product test (2D analogue)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2]		$a
 * @local	float32[2]		$b
 * @local	float32[2]		$c
 * @local	float64[2]		$d
 * @local	float64[][2]	$e
 * @local	float64[2]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5);
	$b = array(1, 2);
	$c = array(10, 5);
	$d = array(-1, -1);
	$e = array(array(1, 1), array(1, 2), array(0, 2), array(8, 5));
	$f = array(1, -1);
	
	echo cross($a, $b), "\n";
	echo cross($c, $d), "\n";
	echo cross($e, $f), "\n";
}

test_function();

?>
--EXPECT--
[0.5, 0.5]
[-5, -5]
[[-2, -2], [-3, -3], [-2, -2], [-13, -13]]
