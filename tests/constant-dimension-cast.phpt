--TEST--
Constant dimension cast test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64[4][4]		$a
 * 
 * @return	void
 * 
 */
function test_function() {
	echo float64(count($a)) + 0.5, "\n";
	echo float64(count($a, true)) + 0.5, "\n";
	echo count($a) + 0.5, "\n";
	echo count($a, true) + 0.5, "\n";
}

test_function();

?>
--EXPECT--
4.5
16.5
4.5
16.5
