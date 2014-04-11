--TEST--
Error reporting test (missing constant in type declaration)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[missing]	$a
 * 
 * @return	void
 * 
 */

function test_function() {
	echo "Hello";
}

test_function();

?>
--EXPECTREGEX--
.*line 7.*