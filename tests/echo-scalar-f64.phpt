--TEST--
Echo variable test (scalar, f32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 3.14159265359;
	echo $a;
}

test_function();

?>
--EXPECT--
3.14159265359
