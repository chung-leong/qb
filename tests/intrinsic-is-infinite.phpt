--TEST--
Infinite check test
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

	echo "is_infinite($a) = ", is_infinite($a), "\n";
	echo "is_infinite($b) = ", is_infinite($b), "\n";
	echo "is_infinite($c) = ", is_infinite($c), "\n";
	echo "is_infinite($d) = ", is_infinite($d), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
is_infinite(-INF) = 1
is_infinite(3.1415926535898) = 0
is_infinite(100) = 0
is_infinite([1, 2, -INF, 0]) = [0, 0, 1, 0]
