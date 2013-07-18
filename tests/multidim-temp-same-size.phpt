--TEST--
Multidimensional temporary variable test (same size)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][4]		$a
 * @local	float64[2][2][2]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	echo $a + 1, "\n";
	echo $a + $b + 2, "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[1, 1, 1, 1], [1, 1, 1, 1]]
[[[2, 2], [2, 2]], [[2, 2], [2, 2]]]
