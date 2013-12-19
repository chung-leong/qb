--TEST--
Rounding test
--SKIPIF--
<?php 
	if(!defined('PHP_ROUND_HALF_UP')) print 'skip PHP_ROUND_HALF_UP is not defined';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$a
 * @local	float64	$b
 * @local	uint32	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1/3;
	$b = M_PI;
	$c = 12345678;
	
	echo round($a), "\n";
	echo round($b), "\n";
	echo round($c), "\n";
	
	echo round($a, 1), "\n";
	echo round($b, 2), "\n";
	echo round($c, -3), "\n";

	$b = 1.05;
	
	echo round($b, 1, PHP_ROUND_HALF_UP), "\n";
	echo round($b, 1, PHP_ROUND_HALF_DOWN), "\n";
	echo round($b, 1, PHP_ROUND_HALF_ODD), "\n";
	echo round($b, 1, PHP_ROUND_HALF_EVEN), "\n";
}

test_function();

?>
--EXPECT--
0
3
12345678
0.3
3.14
12346000
1.1
1
1.1
1
