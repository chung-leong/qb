--TEST--
Matrix-vector multiplication test (row-major)
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
		array(1, 2),
		array(3, 4),
	);
	$v2 = array(1, 2);
	$r2 = mv_mult_rm($m2, $v2);
	echo "$r2\n";
	
	$m3 = array(
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
	);
	$v3 = array(1, 2, 3);
	$r3 = mv_mult_rm($m3, $v3);
	echo "$r3\n";
	
	$m4 = array(
		array( 1,  2,  3,  4),
		array( 5,  6,  7,  8),
		array( 9, 10, 11, 12),
		array(13, 14, 15, 16),
	);
	$v4 = array(1, 2, 3, 4);
	$r4 = mv_mult_rm($m4, $v4);
	echo "$r4\n";
	
	$m5 = array(
		array( 1,  2,  3,  4,  5),
		array( 6,  7,  8,  9, 10),
		array(11, 12, 13, 14, 15),
		array(16, 17, 18, 19, 20),
		array(21, 22, 23, 24, 25),
	);
	$v5 = array(1, 2, 3, 4, 5);
	$r5 = mv_mult_rm($m5, $v5);
	echo "$r5\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[5, 11]
[14, 32, 50]
[30, 70, 110, 150]
[55, 130, 205, 280, 355]
