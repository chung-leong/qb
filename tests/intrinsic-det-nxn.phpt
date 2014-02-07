--TEST--
Matrix determinant test (n > 4)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[5][5]		$m1
 * @local	float32[6][6]		$m2
 * @local	float32				$r
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
	$r = det($m1);
	echo "$r\n";
	
	$m2 = array(
		array(  1,   2,   3,   4,   5,   6),
		array( -7,   8,   9,  10,  11,  12),
		array(-13, -14,  15,  16,  17,  18),
		array(-19, -20, -21,  22,  23,  24),
		array(-25, -26, -27, -28,  29,  30),
		array(-31, -32, -33, -34, -35,  36),
	);
	$r = det($m2);
	echo "$r\n";
}

test_function();

?>
--EXPECT--
1589960
2.377152E+8
