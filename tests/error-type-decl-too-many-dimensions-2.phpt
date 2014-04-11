--TEST--
Error reporting test (too many dimensions, named)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[1][2][3][4][5][6][7][8][a,b,c,d]	$a
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