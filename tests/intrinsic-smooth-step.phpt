--TEST--
Smooth step test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$a
 * @local	float64	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 4;
	echo smooth_step(10, 12, $a), "\n";
	$a = 11;
	echo smooth_step(10, 12, $a), "\n";

	$b = 25;
	echo smooth_step(10, 12, $b), "\n";
	$b = 11;
	echo smooth_step(10, 12, $b), "\n";
	$b = 10.5;
	echo smooth_step(10, 12, $b), "\n";
	$b = 10.25;
	echo smooth_step(10, 12, $b), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
0
0.5
1
0.5
0.15625
0.04296875
