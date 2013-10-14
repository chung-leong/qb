--TEST--
Matrix inverse test
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
	
	$a = inverse($a);
	$b = inverse($b);
	$c = inverse($c);
	$d = inverse($d);
	
	echo "$a\n";
	echo "$b\n";
	echo "$c\n";
	echo "$d\n";
}

test_function();

?>
--EXPECT--
[[1, -0], [-0, 1]]
[[0.1, 0, 0], [0, 0.1, 0], [0, -0.1, 0.1]]
[[-0.125, -0, -0, 0.28125], [0.125, 0.5, -0, -0.28125], [0.125, -0, 0.3333333, -0.28125], [0.125, -0, -0, -0.03125]]
[[[1, -0], [-0, 1]], [[0.2, -0], [-0, 0.2]]]
