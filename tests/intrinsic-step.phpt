--TEST--
Step test
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
	
	echo step(10, $a), "\n";
	echo step(10, $b), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
0
1
