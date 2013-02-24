--TEST--
Echo variable test (scalar, i8)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int8	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 32;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
32
