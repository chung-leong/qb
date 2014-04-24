--TEST--
Matrix-vector multiplication test (column-major)
--FILE--
<?php

ini_set("qb.column_major_matrix", true);

/**
 * @engine qb
 * @local matrix2x4		$m
 * @local vector4		$v
 */
function test_function() {
	$m = array(	array(1, 5),
				array(2, 6),
				array(3, 7),
				array(4, 8) );
	$v = array( 2, 4, 0, 1 );
	echo $m * $v, "\n";
}

test_function();

?>
--EXPECT--
[14, 42]
