--TEST--
Unset test (variable-length array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint8[*]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Hello";
	echo "$a\n";
	unset($a);
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[72, 101, 108, 108, 111]
[]
