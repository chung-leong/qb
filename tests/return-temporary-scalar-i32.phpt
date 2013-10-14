--TEST--
Return temporary variable test (scalar, i32)
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
	return $a - 234;
}

echo test_function();

?>
--EXPECT--
1000
