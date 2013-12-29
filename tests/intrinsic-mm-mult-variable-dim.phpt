--TEST--
Matrix-matrix multiplication test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[?][?]	$m1
 * @param	float32[?][?]	$m2
 * @local	float32[?][?]	$r
 * 
 * @return	void
 * 
 */
function test_function($m1, $m2) {
	$r = mm_mult($m1, $m2);	
	echo "$r\n";
}

ini_set("qb.column_major_matrix", false);

$m3x3 = array(
	array(10,  0,  0),
	array( 0, 10,  0),
	array( 0,  0, 10),
);
$m4x3 = array(
	array(1,  2,  3,  4),
	array(5,  6,  7,  8),
	array(9, 10, 11, 12),
);

test_function($m3x3, $m4x3);
test_function($m4x3, $m3x3);

?>
--EXPECTREGEX--
\[\[10, 20, 30, 40\], \[50, 60, 70, 80\], \[90, 100, 110, 120\]\]
.*Fatal error.*

