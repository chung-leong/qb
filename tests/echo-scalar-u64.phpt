--TEST--
Echo variable test (scalar, u64)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint64	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0x000FFFFFFFFFFFF7; 
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
4503599627370487
