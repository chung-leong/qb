--TEST--
Array expansion test (1 dimension)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[] = 1;
	$a[] = 2;
	$a[] = 3;
	$a[] = 4;
	$a[] = 5;
	$a[] = 6;
	$a[] = 7;
	$a[] = 8;
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5, 6, 7, 8]
