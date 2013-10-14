--TEST--
Array splice test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4, 5, 6, 7);
	array_splice($a, 1, 2, array(17, 18));
	echo "$a\n";
	array_splice($a, 1, 3, array(2, 3));
	echo "$a\n";
	array_splice($a, 4, 0, array(66, 88, 99));
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[1, 17, 18, 4, 5, 6, 7]
[1, 2, 3, 5, 6, 7]
[1, 2, 3, 5, 66, 88, 99, 6, 7]
