--TEST--
Array element presence test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][2]	$a
 * @local	int32[2]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(array(1, -22), array(3, 127), array(-300, 2));
	$b = array(-300, 2);
	echo in_array($a, array(1, -22)), "\n";
	echo in_array($a, array(1, 22)), "\n";
	echo in_array($a, $b), "\n";
	$b += 1;
	echo in_array($a, $b), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1
0
1
0
