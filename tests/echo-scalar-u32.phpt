--TEST--
Echo variable test (scalar, u32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0xFFFFFFFE;
	echo $a;
}

test_function();

?>
--EXPECT--
4294967294
