--TEST--
Echo variable test (scalar, u16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint16	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0xFFFE;
	echo $a;
}

test_function();

?>
--EXPECT--
65534
