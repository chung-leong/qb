--TEST--
Matrix tranpose test (n > 4)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4][5]		$m1
 * @local	float32[5][6]		$m2
 * @local	float32[5][4]		$r1
 * @local	float32[6][5]		$r2
 * 
 * @return	void
 * 
 */
function test_function() {
	$m1 = array(
		array( 1,  2,  3,  4,  5),
		array( 6,  7,  8,  9, 10),
		array(11, 12, 13, 14, 15),
		array(16, 17, 18, 19, 20),
	);
	$r1 = transpose($m1);
	echo "$r1\n";
	
	$m2 = array(
		array( 1,  2,  3,  4,  5,  6),
		array( 7,  8,  9, 10, 11, 12),
		array(13, 14, 15, 16, 17, 18),
		array(19, 20, 21, 22, 23, 24),
		array(25, 26, 27, 28, 29, 30),
	);
	$r2 = transpose($m2);
	echo "$r2\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[1, 6, 11, 16], [2, 7, 12, 17], [3, 8, 13, 18], [4, 9, 14, 19], [5, 10, 15, 20]]
[[1, 7, 13, 19, 25], [2, 8, 14, 20, 26], [3, 9, 15, 21, 27], [4, 10, 16, 22, 28], [5, 11, 17, 23, 29], [6, 12, 18, 24, 30]]
