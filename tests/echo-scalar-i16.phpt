--TEST--
Echo variable test (scalar, i16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int16	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0xFFFE;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
-2
