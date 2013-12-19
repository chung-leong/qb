--TEST--
Power function test
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
	$a = 2;
	$b = 8;
	$b = pow($a, $b);
	$c = 10;
	$d = 4;
	$d = pow($c, $d);
	echo "$b $d\n";
}

test_function();

?>
--EXPECT--
256 10000
