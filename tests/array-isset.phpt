--TEST--
Array isset test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]	$a
 * @local	uint32[*]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	echo isset($a), "\n";
	echo isset($b), "\n";
	$b[] = 1;
	echo isset($b), "\n";
}

test_function();

?>
--EXPECT--
1
0
1

