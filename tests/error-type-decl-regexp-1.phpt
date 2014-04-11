--TEST--
Error reporting test (syntax error in regular expression #1)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$(evil(
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