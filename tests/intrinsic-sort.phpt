--TEST--
Sort test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]			$a
 * @local	float64[][4]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(3, 1, 7, 9, 4, 5, 2, 6, 8);
	echo "$a\n";
	sort($a);
	echo "$a\n";
	rsort($a);
	echo "$a\n";
	
	$b = array(
		array(4, 4, 4, 4),
		array(1, 2, 3, 4),
		array(2, 1, 0, 1),
		array(1, 0, 3, 3),
		array(0.5, 0.3, 1, 4),
		array(3, 0.5, 0.3, 1),
		array(0, 0.5, 0.3, 1),
		array(4, 3, 2, 1),
	);
	echo "$b\n";
	sort($b);
	echo "$b\n";
	rsort($b);
	echo "$b\n";
}

test_function();

?>
--EXPECT--
[3, 1, 7, 9, 4, 5, 2, 6, 8]
[1, 2, 3, 4, 5, 6, 7, 8, 9]
[9, 8, 7, 6, 5, 4, 3, 2, 1]
[[4, 4, 4, 4], [1, 2, 3, 4], [2, 1, 0, 1], [1, 0, 3, 3], [0.5, 0.3, 1, 4], [3, 0.5, 0.3, 1], [0, 0.5, 0.3, 1], [4, 3, 2, 1]]
[[0, 0.5, 0.3, 1], [0.5, 0.3, 1, 4], [1, 0, 3, 3], [1, 2, 3, 4], [2, 1, 0, 1], [3, 0.5, 0.3, 1], [4, 3, 2, 1], [4, 4, 4, 4]]
[[4, 4, 4, 4], [4, 3, 2, 1], [3, 0.5, 0.3, 1], [2, 1, 0, 1], [1, 2, 3, 4], [1, 0, 3, 3], [0.5, 0.3, 1, 4], [0, 0.5, 0.3, 1]]