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
 * @local	float32		$a
 * @local	float64		$b
 * @local	float64		$c
 * @local	uint32		$d
 * @local	float32[4]	$e
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0.3;
	$b = 1.9;
	$c = -7.7;
	$d = 12345678;
	$e = array(32.7, 55.5, 100, -100);
	
	echo floor($a), "\n";
	echo floor($b), "\n";
	echo floor($c), "\n";
	echo floor($d), "\n";
	echo floor($e), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
0
1
-8
12345678
[32, 55, 100, -100]
