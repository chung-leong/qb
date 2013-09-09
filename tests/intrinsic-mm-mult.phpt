--TEST--
Matrix-matrix multiplication test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3][3]		$m
 * @local	float32[3][4]		$a
 * @local	float32[2][3][3]	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$m = array(
		array(10,  0,  0),
		array( 0, 10,  0),
		array( 0,  0, 10),
	);
	$a = array(
		array(1,  2,  3,  4),
		array(5,  6,  7,  8),
		array(9, 10, 11, 12),
	);
	echo mm_mult($m, $a), "\n";	
	
	$c[0] = $m;
	$c[1] = $m * 0.5;
	$c = mm_mult($m, $c);
	echo "$c\n";
}

ini_set("qb.column_major_matrix", false);

qb_compile();

test_function();

?>
--EXPECT--
[[10, 20, 30, 40], [50, 60, 70, 80], [90, 100, 110, 120]]
[[[100, 0, 0], [0, 100, 0], [0, 0, 100]], [[50, 0, 0], [0, 50, 0], [0, 0, 50]]]
