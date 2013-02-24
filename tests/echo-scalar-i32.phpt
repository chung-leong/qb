--TEST--
Echo variable test (scalar, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0xFFFFFFFE;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
-2
