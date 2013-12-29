--TEST--
Matrix determinant test (variable dimension)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64[?][?]	$m
 * @local	float64			$r
 * 
 * @return	void
 * 
 */
function test_function($m) {
	$r = det($m);
	echo "$r\n";
}

$m4x4 = array(
	array(  1,   2,   3,   4),
	array( -5,   6,   7,   8),
	array( -9, -10,  11,  12),
	array(-13, -14, -15,  16),
);

$m4x5 = array(
	array(  1,   2,   3,   4),
	array( -5,   6,   7,   8),
	array( -9, -10,  11,  12),
	array(-13, -14, -15,  16),
	array(-17, -18, -19,  20),
);

ini_set("qb.column_major_matrix", false);

test_function($m4x4);
test_function($m4x5);

?>
--EXPECTREGEX--
16224
.*Fatal error.*

