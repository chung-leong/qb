--TEST--
Matrix inverse test (n > 4)
--FILE--
<?php

ini_set('precision', 8);

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[5][5]		$(m1|r1)
 * @local	float32[6][6]		$(m2|r2)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m1 = array(
		array(  1,   2,   3,   4,   5),
		array( -6,   7,	  8,   9,  10),
		array(-11, -12,  13,  14,  15),
		array(-16, -17, -18,  19,  20),
		array(-21, -22, -23, -24,  25),
	);
	$r1 = inverse($m1);
	echo "$r1\n";
	
	$m2 = array(
		array(  1,   2,   3,   4,   5,   6),
		array( -7,   8,   9,  10,  11,  12),
		array(-13, -14,  15,  16,  17,  18),
		array(-19, -20, -21,  22,  23,  24),
		array(-25, -26, -27, -28,  29,  30),
		array(-31, -32, -33, -34, -35,  36),
	);
	$r2 = inverse($m2);
	echo "$r2\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[0.2392, -0.09628, -0.005975, -0.003774, -0.002717], [-0.02174, 0.07531, -0.04216, -0.0003145, -0.0002264], [-0.008453, 0.001509, 0.03916, -0.0279, -8.805E-5], [-0.004579, 0.0008176, 0.0003774, 0.02655, -0.02088], [0.1696, -0.01243, -0.005736, -0.003623, 0.01739]]
[[0.2091, -0.08137, -0.004638, -0.002951, -0.002137, -0.001662], [-0.02078, 0.06562, -0.03605, -0.0002108, -0.0001527, -0.0001187], [-0.007915, 0.001187, 0.03389, -0.02389, -5.815E-5, -4.523E-5], [-0.00424, 0.0006361, 0.0002968, 0.02292, -0.01789, -2.423E-5], [-0.002665, 0.0003998, 0.0001866, 0.0001187, 0.01733, -0.0143], [0.1477, -0.009662, -0.004509, -0.002869, -0.002078, 0.01227]]