--TEST--
Echo variable test (scalar, f32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 3.25;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
3.25
