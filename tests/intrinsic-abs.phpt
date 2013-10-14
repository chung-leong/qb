--TEST--
Abs test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$a
 * @local	float64	$b
 * @local	int32	$c
 * @local	uint16	$d
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0.1;
	$b = -8.5;
	$c = -8;
	$d = 0xFFFF;
	
	echo abs($a), "\n";
	echo abs($b), "\n";
	echo abs($c), "\n";
	echo abs($d), "\n";
}

test_function();

?>
--EXPECT--
0.1
8.5
8
1
