--TEST--
Matrix-matrix multiplication test (column-major)
--FILE--
<?php

ini_set("qb.column_major_matrix", false);

/**
 * @engine qb
 * @local matrix2x4		$m1
 * @local matrix4x4		$m2
 * @local matrix2x2		$m3
 */
function test_function() {
	$m1 = array(	array(1, 5),
					array(2, 6),
					array(3, 7),
					array(4, 8) );
	$m2 = array(	array(2, 0, 0, 0),
					array(0, 2, 0, 0),
					array(0, 0, 2, 0),
					array(0, 0, 0, 2)	);
	$m3 = array(	array(2, 0),
					array(0, 2)	);
	echo $m1 * $m2, "\n";
	echo $m3 * $m1, "\n";
}

test_function();

?>
--EXPECT--
[[2, 4, 6, 8], [10, 12, 14, 16]]
[[2, 4, 6, 8], [10, 12, 14, 16]]

