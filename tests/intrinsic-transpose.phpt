--TEST--
Matrix tranpose test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$a
 * @local	float32[3][3]		$b
 * @local	float32[4][4]		$c
 * @local	float32[2][2][2]	$d
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(
		array(1, 2),
		array(3, 4),
	);
	$b = array(
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
	);
	$c = array(
		array( 1,  2,  3,  4),
		array( 5,  6,  7,  8),
		array( 9, 10, 11, 12),
		array(13, 14, 15, 16),
	);
	$d = array( 
		array(
			array(1, 2),
			array(3, 4),
		),
		array(
			array(5, 6),
			array(7, 8),
		),
	);
	
	$a = transpose($a);
	$b = transpose($b);
	$c = transpose($c);
	$d = transpose($d);
	
	echo "$a\n";
	echo "$b\n";
	//echo "$c\n";
	//echo "$d\n";
}

test_function();

?>
--EXPECT--
[[1, 3], [2, 4]]
[[1, 4, 7], [2, 5, 8], [3, 6, 9]]
[[1, 5, 9, 13], [2, 6, 10, 14], [3, 7, 11, 15], [4, 8, 12, 16]]
[[[1, 3], [2, 4]], [[5, 7], [6, 8]]]
