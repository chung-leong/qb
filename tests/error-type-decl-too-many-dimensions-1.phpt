--TEST--
Error reporting test (too many dimensions)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[1][2][3][4][5][6][7][8][9]	$a
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