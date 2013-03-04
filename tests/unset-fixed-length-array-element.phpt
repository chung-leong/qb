--TEST--
Unset test (element in fixed-length array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint8[5]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Hello";
	echo "$a\n";
	unset($a[1]);
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[72, 101, 108, 108, 111]
[72, 108, 108, 111, 0]
