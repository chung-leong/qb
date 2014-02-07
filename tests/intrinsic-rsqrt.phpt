--TEST--
Reciprocal square root test
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
	$b = 25;
	
	echo rsqrt($a), "\n";
	echo rsqrt($b), "\n";
}

test_function();

?>
--EXPECT--
0.5
0.2
