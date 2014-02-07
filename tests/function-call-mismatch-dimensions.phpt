--TEST--
Function call array dimension mismatch test 
--FILE--
<?php

/**
 * @engine	qb
 * @inline	never
 * @param	int32[4][8]			$a
 * @param	int32[][3][4]		$b
 */
function other_function($a, $b) {
	echo "$a\n";
	echo "$b\n";
}

/**
 * @engine	qb
 * @local	int32[2][6]			$a
 * @local	int32[2][4][3]		$b
 */
function test_function() {
	$a = array(
		array( 1,  2,  3,  4,  5,  6),
		array( 7,  8,  9, 10, 11, 12),
	);
	$b = array(
		array(array( 1,  2,  3), array( 4,  5,  6), array( 7,  8,  9), array(10, 11, 12)),
		array(array(13, 14, 15), array(16, 17, 18), array(19, 20, 21), array(22, 23, 24)),
	);
	other_function($a, $b);
}

test_function();

?>
--EXPECT--
[[1, 2, 3, 4, 5, 6, 7, 8], [9, 10, 11, 12, 1, 2, 3, 4], [5, 6, 7, 8, 9, 10, 11, 12], [1, 2, 3, 4, 5, 6, 7, 8]]
[[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]], [[13, 14, 15, 16], [17, 18, 19, 20], [21, 22, 23, 24]]]

