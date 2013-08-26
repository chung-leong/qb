--TEST--
Array column test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[3][2]		$a
 * @local	int32[3][2][2]	$b
 * @local	int32[3]		$r1
 * @local	int32[*]		$r2
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(
			array(1, 2), 
			array(3, 4), 
			array(5, 6)
	);
	$b = array(
			array(array(1, 2), array(3, 4)), 
			array(array(5, 6), array(7, 8)),
			array(array(9, 10), array(11, 12)),
	);
	$r1 = array_column($a, 0);
	echo "$r1\n";
	$r2 = array_column($a, 1);
	echo "$r2\n";
	echo array_column($a, 1), "\n";
	echo array_column($b, 0), "\n";
	echo array_column($b, 1), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 3, 5]
[2, 4, 6]
[2, 4, 6]
[[1, 2], [5, 6], [9, 10]]
[[3, 4], [7, 8], [11, 12]]
