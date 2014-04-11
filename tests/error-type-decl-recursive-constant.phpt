--TEST--
Error reporting test (recursive constant in type declaration)
--FILE--
<?php

define('NAME', 'another');

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[NAME]	$a
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
.*line 9.*