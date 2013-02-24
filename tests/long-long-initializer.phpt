--TEST--
64 bit integer initializer
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int64	$a
 * @local	uint64	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = int64(0xFFFFFFFFF, 0xFFFFFFFF);
	$b = int64((1 << 6) + 1, 0x0F | 0xF0);
	echo "$a $b\n";
}

qb_compile();

test_function();

?>
--EXPECT--
-1 279172874495