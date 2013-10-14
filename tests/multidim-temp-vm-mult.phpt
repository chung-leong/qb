--TEST--
Vector-matrix multiplication test (temporary)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2]			$v1
 * @local	float32[4][2]		$v2
 * @local	float64[2][2]		$m1
 * @local	float64[2][2][2]	$m2
 * 
 * @return	void
 * 
 */
function test_function() {
	$v1 = array(2.5, 4.5);
	$v2 = array(array(1, 2), array(2, 3), array(3, 4), array(4, 5));
	
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

	echo vm_mult($v1, $m1), "\n";
	echo vm_mult($v2, $m1), "\n";
	
	echo vm_mult($v1, $m2), "\n";
	echo vm_mult($v2, $m2), "\n";
}

ini_set("qb.column_major_matrix", true);

test_function();

?>
--EXPECT--
[2.5, 4.5]
[[1, 2], [2, 3], [3, 4], [4, 5]]
[[2.5, 4.5], [5, 9]]
[[1, 2], [4, 6], [3, 4], [8, 10]]
