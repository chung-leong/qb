--TEST--
Array shift test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * @local	int32[*][2]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4);
	echo array_shift($a), "\n";
	echo $a, "\n";
	
	$b = array(array(1, 2), array(3, 4), array(5, 6));
	echo array_shift($b), "\n";
	echo $b, "\n";
}

test_function();

?>
--EXPECT--
1
[2, 3, 4]
[1, 2]
[[3, 4], [5, 6]]
