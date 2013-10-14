--TEST--
Matrix-matrix multiplication test (column-major)
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
		array(1, 3),
		array(2, 4),
	);
	$n2 = $m2 * 0.1;
	$r2 = mm_mult_cm($m2, $n2);
	echo "$r2\n";
	
	$m3 = array(
		array(1, 4, 7),
		array(2, 5, 8),
		array(3, 6, 9),
	);
	$n3 = $m3 * 0.1;
	$r3 = mm_mult_cm($m3, $n3);
	echo "$r3\n";
	
	$m4 = array(
		array( 1,  5,  9, 13),
		array( 2,  6, 10, 14),
		array( 3,  7, 11, 15),
		array( 4,  8, 12, 16),
	);
	$n4 = $m4 * 0.1;
	$r4 = mm_mult_cm($m4, $n4);
	echo "$r4\n";
	
	$m5 = array(
		array( 1,  6, 11, 16, 21),
		array( 2,  7, 12, 17, 22),
		array( 3,  8, 13, 18, 23),
		array( 4,  9, 14, 19, 24),
		array( 5, 10, 15, 20, 25),
	);
	$n5 = $m5 * 0.1;
	$r5 = mm_mult_cm($m5, $n5);
	echo "$r5\n";
}

test_function();

?>
--EXPECT--
[[0.7, 1.5], [1, 2.2]]
[[3, 6.6, 10.2], [3.6, 8.1, 12.6], [4.2, 9.6, 15]]
[[9, 20.2, 31.4, 42.6], [10, 22.8, 35.6, 48.4], [11, 25.4, 39.8, 54.2], [12, 28, 44, 60]]
[[21.5, 49, 76.5, 104, 131.5], [23, 53, 83, 113, 143], [24.5, 57, 89.5, 122, 154.5], [26, 61, 96, 131, 166], [27.5, 65, 102.5, 140, 177.5]]
