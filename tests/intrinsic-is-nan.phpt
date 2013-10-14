--TEST--
Not-a-number check test
--SKIPIF--
<?php
	if(!is_nan(NAN)) print "skip borken NAN constant";
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32		$a
 * @local	float64		$b
 * @local	uint32		$c
 * @local	float64[4]	$d
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = NAN;
	$b = acos(8);
	$c = 100;
	$d[0] = 1;
	$d[1] = 2;
	$d[2] = $a;

	echo "is_nan($a) = ", is_nan($a), "\n";
	echo "is_nan($b) = ", is_nan($b), "\n";
	echo "is_nan($c) = ", is_nan($c), "\n";
	echo "is_nan($d) = ", is_nan($d), "\n";
}

test_function();

?>
--EXPECT--
is_nan(NAN) = 1
is_nan(NAN) = 1
is_nan(100) = 0
is_nan([1, 2, NAN, 0]) = [0, 0, 1, 0]
