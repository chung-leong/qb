--TEST--
Matrix-vector multiplication test (column-major)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$m2
 * @local	float32[3][3]		$m3
 * @local	float32[4][4]		$m4
 * @local	float32[5][5]		$m5
 * @local	float32[2]			$(v2|r2)
 * @local	float32[3]			$(v3|r3)
 * @local	float32[4]			$(v4|r4)
 * @local	float32[5]			$(v5|r5)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m2 = array(
		array(1, 3),
		array(2, 4),
	);
	$v2 = array(1, 2);
	$r2 = mv_mult_cm($m2, $v2);
	echo "$r2\n";
	
	$m3 = array(
		array(1, 4, 7),
		array(2, 5, 8),
		array(3, 6, 9),
	);
	$v3 = array(1, 2, 3);
	$r3 = mv_mult_cm($m3, $v3);
	echo "$r3\n";
	
	$m4 = array(
		array( 1,  5,  9, 13),
		array( 2,  6, 10, 14),
		array( 3,  7, 11, 15),
		array( 4,  8, 12, 16),
	);
	$v4 = array(1, 2, 3, 4);
	$r4 = mv_mult_cm($m4, $v4);
	echo "$r4\n";
	
	$m5 = array(
		array( 1,  6, 11, 16, 21),
		array( 2,  7, 12, 17, 22),
		array( 3,  8, 13, 18, 23),
		array( 4,  9, 14, 19, 24),
		array( 5, 10, 15, 20, 25),
	);
	$v5 = array(1, 2, 3, 4, 5);
	$r5 = mv_mult_cm($m5, $v5);
	echo "$r5\n";
}

test_function();

?>
--EXPECT--
[5, 11]
[14, 32, 50]
[30, 70, 110, 150]
[55, 130, 205, 280, 355]
