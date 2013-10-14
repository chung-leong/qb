--TEST--
Unset test (variable)
--FILE--
<?php

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
	$a = 5;
	echo "$a\n";
	unset($a);
	echo "$a\n";
}

test_function();

?>
--EXPECT--
5
0
