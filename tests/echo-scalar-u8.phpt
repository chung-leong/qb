--TEST--
Echo variable test (scalar, u8)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint8	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0xFE;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
254
