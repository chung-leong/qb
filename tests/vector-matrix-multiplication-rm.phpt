--TEST--
Vector-matrix multiplication test (row-major)
--FILE--
<?php

ini_set("qb.column_major_matrix", false);

/**
 * @engine qb
 * @local matrix2x4		$m
 * @local vector2		$v
 */
function test_function() {
	$m = array(	array(1, 2, 3, 4),
				array(5, 6, 7, 8) );
	$v = array( 2, 4 );
	echo $v * $m, "\n";
}

test_function();

?>
--EXPECT--
[22, 28, 34, 40]

