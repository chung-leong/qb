--TEST--
Matrix inverse test (row-major)
--FILE--
<?php

ini_set('precision', 8);

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$(m2|r2)
 * @local	float32[3][3]		$(m3|r3)
 * @local	float32[4][4]		$(m4|r4)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m2 = array(
		array( 1, -3),
		array( 2,  4),
	);
	$r2 = inverse_cm($m2);
	echo "$r2\n";
	
	$m3 = array(
		array( 1, -4, -7),
		array( 2,  5, -8),
		array( 3,  6,  9),
	);
	$r3 = inverse_cm($m3);
	echo "$r3\n";
	
	$m4 = array(
		array(  1,  -5,  -9, -13),
		array(  2,   6, -10, -14),
		array(  3,   7,  11, -15),
		array(  4,   8,  12,  16),
	);
	$r4 = inverse_cm($m4);
	echo "$r4\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[0.4, 0.3], [-0.2, 0.1]]
[[0.3298, -0.02128, 0.2376], [-0.1489, 0.1064, -0.02128], [-0.01064, -0.06383, 0.0461]]
[[0.2781, -0.02219, -0.008876, 0.1982], [-0.1174, 0.08826, 0.001972, -0.01627], [-0.00789, -0.05079, 0.04635, -0.007396], [-0.004931, -0.0004931, -0.03353, 0.02663]]
