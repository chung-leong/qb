--TEST--
Rounding test (rint)
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
	$a = 1.5;
	$b = 2.5;
	echo rint($a), "\n";
	echo rint($b), "\n";

	$a = 2.5;
	$b = 3.5;
	echo rint($a), "\n";
	echo rint($b), "\n";
}

test_function();

?>
--EXPECT--
2
2
2
4

