--TEST--
Fract test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$a
 * @local	float64	$[bc]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0.1;
	$b = -8.5;
	$c = 0;
	
	echo fract($a), "\n";
	echo fract($b), "\n";
	echo fract($c), "\n";
}

test_function();

?>
--EXPECT--
0.1
0.5
0
