--TEST--
Array splice test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]			$a
 * @local	float32[][3]	$b;
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4, 5, 6, 7);
	echo array_splice($a, 1, 2, array(17, 18)), "\n";
	echo "$a\n";
	array_splice($a, 1, 3, array(2, 3));
	echo "$a\n";
	array_splice($a, 4, 0, array(66, 88, 99));
	echo "$a\n";
	
	$b = array(array(1, 2, 3), array(4, 5, 6));
	echo array_splice($b, 1, 0, array(3, 4, 3)), "\n";
	echo "$b\n";
	echo array_splice($b, -2, 1), "\n";
	echo "$b\n";
}

test_function();

?>
--EXPECT--
[2, 3]
[1, 17, 18, 4, 5, 6, 7]
[1, 2, 3, 5, 6, 7]
[1, 2, 3, 5, 66, 88, 99, 6, 7]
[]
[[1, 2, 3], [3, 4, 3], [4, 5, 6]]
[[3, 4, 3]]
[[1, 2, 3], [4, 5, 6]]