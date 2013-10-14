--TEST--
Zend function call test (scalar return value)
--FILE--
<?php

function other_function() {
	return 1234;
}

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = other_function();
	echo "$a\n";
}

test_function();

?>
--EXPECT--
1234
