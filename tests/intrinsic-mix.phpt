--TEST--
Mix (linear interpolation) test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	echo mix(50, 100, 0.9), "\n";
}

test_function();

?>
--EXPECT--
95
