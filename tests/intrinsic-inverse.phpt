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
		array(10, 10, 10),
		array(10, 10, 10),
		array(10, 10, 10),
	);
	$c = array(
		array(5, 4, 3, 2),
		array(1, 1, 1, 1),
		array(1, 2, 3, 4),
		array(5, 6, 7, 8),
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

qb_compile();

test_function();

?>
--EXPECT--
