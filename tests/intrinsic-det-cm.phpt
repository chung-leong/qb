--TEST--
Matrix determinant test (column-major)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64[2][2]		$m2
 * @local	float64[3][3]		$m3
 * @local	float64[4][4]		$m4
 * @local	float64				$(r2|r3|r4)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m2 = array(
		array( 1, -3),
		array( 2,  4),
	);
	$r2 = det_cm($m2);
	echo "$r2\n";
	
	$m3 = array(
		array( 1, -4, -7),
		array( 2,  5, -8),
		array( 3,  6,  9),
	);
	$r3 = det_cm($m3);
	echo "$r3\n";
	
	$m4 = array(
		array(  1,  -5,  -9, -13),
		array(  2,   6, -10, -14),
		array(  3,   7,  11, -15),
		array(  4,   8,  12,  16),
	);
	$r4 = det_cm($m4);
	echo "$r4\n";
}

test_function();

?>
--EXPECT--
10
282
16224
