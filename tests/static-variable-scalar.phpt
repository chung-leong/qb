--TEST--
Static variable test (scalar)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @static	int32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	static $a = 2000;
	echo "$a\n";
	$a++;
}

test_function();
test_function();
test_function();

?>
--EXPECT--
2000
2001
2002