--TEST--
Log10 test
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
	$a = 1000;
	$b = log10($a);
	$c = 10000;
	$d = log10($c);
	echo "$b $d\n";
}

test_function();

?>
--EXPECT--
3 4
