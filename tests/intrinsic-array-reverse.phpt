--TEST--
Array reversal test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]		$a
 * @local	int32[][4]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	echo "$a\n";
	$a = array_reverse($a);
	echo "$a\n";

	$b = array(	
		array(1, 2, 3, 4), 
		array(5, 6, 7, 8), 
		array(9, 10, 11, 12), 
		array(13, 14, 15, 16)
	);
	echo "$b\n";
	$b = array_reverse($b);
	echo "$b\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
[10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]
[[13, 14, 15, 16], [9, 10, 11, 12], [5, 6, 7, 8], [1, 2, 3, 4]]
