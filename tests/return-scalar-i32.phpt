--TEST--
Return variable test (scalar, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$.*
 * 
 * @return	int32
 * 
 */
function test_function() {
	$a = 1234;
	return $a;
}

qb_compile();

echo test_function();

?>
--EXPECT--
1234
