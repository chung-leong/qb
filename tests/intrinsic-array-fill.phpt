--TEST--
Array fill test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64[8]		$r1
 * 
 * @return	void
 * 
 */
function test_function() {
	$r1 = array_fill(0, 8, 17);
	echo "$r1\n";
	$r1 = array_fill(2, 6, 18);
	echo "$r1\n";
	
	echo array_fill(0, 5, array(1, 2)), "\n";
	echo array_fill(5, 5, array(1, 2)), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[17, 17, 17, 17, 17, 17, 17, 17]
[0, 0, 18, 18, 18, 18, 18, 18]
[[1, 2], [1, 2], [1, 2], [1, 2], [1, 2]]
[[0, 0], [0, 0], [0, 0], [0, 0], [0, 0], [1, 2], [1, 2], [1, 2], [1, 2], [1, 2]]
