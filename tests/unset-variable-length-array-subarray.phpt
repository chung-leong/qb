--TEST--
Unset test (sub-array in variable-length array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*][4]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[] = array(1, 2, 3, 4);
	$a[] = array(5, 6, 7, 8);
	$a[] = array(9, 10, 11, 12);
	echo "$a\n";
	unset($a[1]);
	echo "$a\n";
	unset($a[1]);
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]
[[1, 2, 3, 4], [9, 10, 11, 12]]
[[1, 2, 3, 4]]
