--TEST--
Finite check test
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
	$a = log(0);
	$b = M_PI;
	$c = 100;
	$d[0] = 1;
	$d[1] = 2;
	$d[2] = $a;

	echo "is_finite($a) = ", is_finite($a), "\n";
	echo "is_finite($b) = ", is_finite($b), "\n";
	echo "is_finite($c) = ", is_finite($c), "\n";
	echo "is_finite($d) = ", is_finite($d), "\n";
}

test_function();

?>
--EXPECT--
is_finite(-INF) = 0
is_finite(3.1415926535898) = 1
is_finite(100) = 1
is_finite([1, 2, -INF, 0]) = [1, 1, 0, 1]