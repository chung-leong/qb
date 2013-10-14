--TEST--
2 exponential test
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
	$b = exp2($a);
	$c = 16;
	$d = exp2($c);
	echo "$b $d\n";
}

test_function();

?>
--EXPECT--
4 65536
