--TEST--
E exponential test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64	$[ab]
 * @local	float32	$[cd]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1;
	$b = exp($a);
	$c = 1;
	$d = exp($c);
	echo "$b $d\n";
}

test_function();

?>
--EXPECT--
2.718281828459 2.718282
