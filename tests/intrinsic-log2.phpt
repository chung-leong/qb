--TEST--
Log2 test
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
	$a = 8;
	$b = log2($a);
	$c = 16;
	$d = log2($c);
	echo "$b $d\n";
}

test_function();

?>
--EXPECT--
3 4
