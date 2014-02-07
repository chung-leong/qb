--TEST--
Matrix-matrix multiplication test (temporary)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64[2][2]		$m1
 * @local	float64[2][2][2]	$m2
 * @local	float64[2][3]		$m3
 * 
 * @return	void
 * 
 */
function test_function() {
	$m1 = array(
		array(1, 0),
		array(0, 1),
	);

	$m2 = array(
		array(
			array(1, 0),
			array(0, 1)
		),
		array(
			array(2, 0),
			array(0, 2)
		),
	);
	
	$m3 = array(
		array(1, 2, 3),
		array(4, 5, 6),
	);

	echo mm_mult($m1, $m2), "\n";
	echo mm_mult($m3, $m1), "\n";
	echo mm_mult($m3, $m2), "\n";
}

ini_set("qb.column_major_matrix", true);

test_function();

?>
--EXPECT--
[[[1, 0], [0, 1]], [[2, 0], [0, 2]]]
[[1, 2, 3], [4, 5, 6]]
[[[1, 2, 3], [4, 5, 6]], [[2, 4, 6], [8, 10, 12]]]
