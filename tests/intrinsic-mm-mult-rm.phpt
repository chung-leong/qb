--TEST--
Matrix-matrix multiplication test (row-major)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$(m2|n2|r2)
 * @local	float32[3][3]		$(m3|n3|r3)
 * @local	float32[4][4]		$(m4|n4|r4)
 * @local	float32[5][5]		$(m5|n5|r5)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m2 = array(
		array(1, 2),
		array(3, 4),
	);
	$n2 = $m2 * 0.1;
	$r2 = mm_mult_rm($m2, $n2);
	echo "$r2\n";
	
	$m3 = array(
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
	);
	$n3 = $m3 * 0.1;
	$r3 = mm_mult_rm($m3, $n3);
	echo "$r3\n";
	
	$m4 = array(
		array( 1,  2,  3,  4),
		array( 5,  6,  7,  8),
		array( 9, 10, 11, 12),
		array(13, 14, 15, 16),
	);
	$n4 = $m4 * 0.1;
	$r4 = mm_mult_rm($m4, $n4);
	echo "$r4\n";
	
	$m5 = array(
		array( 1,  2,  3,  4,  5),
		array( 6,  7,  8,  9, 10),
		array(11, 12, 13, 14, 15),
		array(16, 17, 18, 19, 20),
		array(21, 22, 23, 24, 25),
	);
	$n5 = $m5 * 0.1;
	$r5 = mm_mult_rm($m5, $n5);
	echo "$r5\n";
}

test_function();

?>
--EXPECT--
[[0.7, 1], [1.5, 2.2]]
[[3, 3.6, 4.2], [6.6, 8.1, 9.6], [10.2, 12.6, 15]]
[[9, 10, 11, 12], [20.2, 22.8, 25.4, 28], [31.4, 35.6, 39.8, 44], [42.6, 48.4, 54.2, 60]]
[[21.5, 23, 24.5, 26, 27.5], [49, 53, 57, 61, 65], [76.5, 83, 89.5, 96, 102.5], [104, 113, 122, 131, 140], [131.5, 143, 154.5, 166, 177.5]]
