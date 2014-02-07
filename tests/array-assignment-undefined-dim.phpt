--TEST--
Array assignment test (undefined dimension)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][]		$a
 * @local	int32[4][3]		$b
 * @local	int32[5][2]		$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$b = array( 
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
		array(10, 11, 12),
	);
	
	$c = array(
		array(1, 2), 
		array(3, 4),
		array(5, 6), 
		array(7, 8),
		array(9, 10),
	);
	
	$a = $b;
	echo $a, "\n";
	
	$a = $c;
	echo $a, "\n";
}

test_function();

?>
--EXPECT--
[[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]]
[[1, 2], [3, 4], [5, 6], [7, 8], [9, 10]]
