--TEST--
Matrix determinant test (row-major)
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
		array( 1,  2),
		array(-3,  4),
	);
	$r2 = det_rm($m2);
	echo "$r2\n";
	
	$m3 = array(
		array( 1,  2,  3),
		array(-4,  5,  6),
		array(-7, -8,  9),
	);
	$r3 = det_rm($m3);
	echo "$r3\n";
	
	$m4 = array(
		array(  1,   2,   3,   4),
		array( -5,   6,   7,   8),
		array( -9, -10,  11,  12),
		array(-13, -14, -15,  16),
	);
	$r4 = det_rm($m4);
	echo "$r4\n";
}

test_function();

?>
--EXPECT--
10
282
16224
