--TEST--
Array difference test
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
	$a = array(1, 2, 3, 3, 4, 9, 7);
	$b = array(1, 2, 3);
	$c = array(10, 4.5);
	$r1 = array_diff($a, $b);
	echo "$r1\n";
	echo array_diff($a, $b), "\n";
	echo array_diff($a, $b, $c), "\n";
	echo array_diff($a, $b, $c, 9, 10, 11, 12, 13), "\n";
	
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
	echo array_diff($d, $e), "\n";
	echo array_diff($d, $e, array(array(9, 10))), "\n";
}

test_function();

?>
--EXPECT--
[4, 9, 7]
[4, 9, 7]
[9, 7]
[7]
[[1, 2], [5, 6], [9, 10]]
[[1, 2], [5, 6]]
