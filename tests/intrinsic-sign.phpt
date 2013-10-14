--TEST--
Clamp test
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
	$b = -8;
	$c = 0;
	
	echo sign($a), "\n";
	echo sign($b), "\n";
	echo sign($c), "\n";
}

test_function();

?>
--EXPECT--
1
-1
0
