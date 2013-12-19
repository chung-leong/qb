--TEST--
Zend function call test (array return value)
--FILE--
<?php

function other_function() {
	$a = array(1, 2, 3, 4);
	return $a;
}

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[10]	$a
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
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2]
