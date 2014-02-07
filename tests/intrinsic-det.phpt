--TEST--
Matrix determinant test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$a
 * @local	float32[3][3]		$b
 * @local	float32[4][4]		$c
 * @local	float32[2][2][2]	$d
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(
		array(1, 0),
		array(0, 1),
	);
	$b = array(
		array(10,  0,  0),
		array( 0, 10,  0),
		array( 0, 10, 10),
	);
	$c = array(
		array(1, 0, 0, 9),
		array(2, 2, 0, 0),
		array(3, 0, 3, 0),
		array(4, 0, 0, 4),
	);
	$d = array( 
		array(
			array(1, 0),
			array(0, 1),
		),
		array(
			array(5, 0),
			array(0, 5),
		),
	);
	
	echo det($a), "\n";
	echo det($b), "\n";
	echo det($c), "\n";
	echo det($d), "\n";
}

ini_set("qb.column_major_matrix", true);

test_function();

?>
--EXPECT--
1
1000
-192
[1, 25]
