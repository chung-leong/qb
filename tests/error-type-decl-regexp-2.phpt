--TEST--
Error reporting test (syntax error in regular expression #2)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$?abc
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Hello";
}

test_function();

?>
--EXPECTREGEX--
.*line 7.*