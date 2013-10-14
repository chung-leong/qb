--TEST--
Echo variable test (element, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[16]	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[1] = 1234;
	echo $a[0], $a[1];
}

test_function();

?>
--EXPECT--
01234
