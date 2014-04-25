--TEST--
Matrix-matrix multiplication test (column-major)
--FILE--
<?php

ini_set("qb.column_major_matrix", true);

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
[[2, 10], [4, 12], [6, 14], [8, 16]]
[[2, 10], [4, 12], [6, 14], [8, 16]]
