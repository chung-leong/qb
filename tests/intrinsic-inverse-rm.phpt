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
		array( 1,  2),
		array(-3,  4),
	);
	$r2 = inverse_rm($m2);
	echo "$r2\n";
	
	$m3 = array(
		array( 1,  2,  3),
		array(-4,  5,  6),
		array(-7, -8,  9),
	);
	$r3 = inverse_rm($m3);
	echo "$r3\n";
	
	$m4 = array(
		array(  1,   2,   3,   4),
		array( -5,   6,   7,   8),
		array( -9, -10,  11,  12),
		array(-13, -14, -15,  16),
	);
	$r4 = inverse_rm($m4);
	echo "$r4\n";
}

test_function();

?>
--EXPECT--
[[0.4, -0.2], [0.3, 0.1]]
[[0.3298, -0.1489, -0.01064], [-0.02128, 0.1064, -0.06383], [0.2376, -0.02128, 0.0461]]
[[0.2781, -0.1174, -0.00789, -0.004931], [-0.02219, 0.08826, -0.05079, -0.0004931], [-0.008876, 0.001972, 0.04635, -0.03353], [0.1982, -0.01627, -0.007396, 0.02663]]
