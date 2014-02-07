--TEST--
Sort test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64[4][4]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(
			array(3, 1, 7, 9), 
			array(4, 5, 2, 6),
			array(2, 3, 1, 8),
			array(3, 4, 9, 1),
		);
	sort($a[1]);
	rsort($a[2]);
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[[3, 1, 7, 9], [2, 4, 5, 6], [8, 3, 2, 1], [3, 4, 9, 1]]
