--TEST--
Matrix-vector multiplication test (variable dimensions)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[?][?]	$m
 * @param	float32[?]	$v
 * @local	float32[?]	$r
 * 
 * @return	void
 * 
 */
function test_function($m, $v) {
	$r = mv_mult($m, $v);
	echo "$r\n";
}

$m5x5 = array(
	array( 1,  2,  3,  4,  5),
	array( 6,  7,  8,  9, 10),
	array(11, 12, 13, 14, 15),
	array(16, 17, 18, 19, 20),
	array(21, 22, 23, 24, 25),
);
$m5x6 = array(
	array( 1,  2,  3,  4,  5),
	array( 6,  7,  8,  9, 10),
	array(11, 12, 13, 14, 15),
	array(16, 17, 18, 19, 20),
	array(21, 22, 23, 24, 25),
	array(26, 27, 28, 29, 30),
);
$v5 = array(1, 2, 3, 4, 5);
$v4 = array(1, 2, 3, 4);

ini_set("qb.column_major_matrix", false);

test_function($m5x5, $v5);
test_function($m5x6, $v5);
test_function($m5x5, $v4);

?>
--EXPECTREGEX--
\[55, 130, 205, 280, 355\]
\[55, 130, 205, 280, 355, 430\]
.*Fatal error.*

