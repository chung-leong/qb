--TEST--
Array intersect test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[8]		$a
 * @local	int32[3]		$b
 * @local	float32[2]		$c
 * @local	int32[5][2]		$d
 * @local	int32[5][2]		$e
 * @local	int32[*]		$r1
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 1, 3, 3, 4, 9, 7);
	$b = array(1, 2, 3);
	$c = array(2.1, 3.5);
	$r1 = array_intersect($a, $b);
	echo "$r1\n";
	echo array_intersect($a, $b), "\n";
	echo array_intersect($a, $b, $c), "\n";
	
	$d = array(
			array(1, 2), 
			array(3, 4), 
			array(5, 6),
			array(7, 8),
			array(9, 10),
	);
	$e = array(
			array(2, 1), 
			array(3, 4), 
			array(6, 5),
			array(7, 8),
			array(10, 9),
	);
	echo array_intersect($d, $e), "\n";
}

test_function();

?>
--EXPECT--
[1, 2, 1, 3, 3]
[1, 2, 1, 3, 3]
[2, 3, 3]
[[3, 4], [7, 8]]
