--TEST--
Array resize test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][]		$a
 * 
 * @return	void
 * 
 */
function test_function() {
	echo "$a\n";
	array_resize($a, 2, 2);
	echo "$a\n";
	$a = 1;
	echo "$a\n";
	array_resize($a, 3, 3);
	echo "$a\n";
	$a = array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9));
	echo "$a\n";
	array_resize($a, 2, 2);
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[]
[[0, 0], [0, 0]]
[[1, 1], [1, 1]]
[[1, 1, 0], [1, 1, 0], [0, 0, 0]]
[[1, 2, 3], [4, 5, 6], [7, 8, 9]]
[[1, 2], [4, 5]]
