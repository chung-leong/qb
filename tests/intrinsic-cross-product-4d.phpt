--TEST--
Cross product test (4D analogue)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]		$a
 * @local	float32[4]		$b
 * @local	float32[4]		$c
 * @local	float64[4]		$d
 * @local	float64[][4]	$e
 * @local	float64[4]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5, 0.5);
	$b = array(1, 2, 3, 4);
	$c = array(10, 5, 0.5, 2);
	$d = array(-1, 1, -1, 1);
	$e = array(array(1, 1, 1, 1), array(1, 2, 3, 4), array(0, 2, -4, -8), array(8, 5, 0, 0));
	$f = array(1, -1, 0.5, 1);
	
	echo cross($a, $b, $c), "\n";
	echo cross($c, $d, $a), "\n";
	echo cross($e, $f, $a), "\n";
}

test_function();

?>
--EXPECT--
[3, -6.25, 3.5, -0.25]
[-3, 9.5, 3, -9.5]
[[0, 0, 0, 0], [-0.5, -0.75, 3, -1.75], [1.5, 2, -8, 4.5], [-1.25, 2, -8, 7.25]]
