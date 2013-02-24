--TEST--
Inverse square root test
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
	
	echo inv_sqrt($a), "\n";
	echo inv_sqrt($b), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
0.5
0.2
