--TEST--
Array element presence test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]	$a
 * @local	float64[]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, -22, 3, 127, -300, 2);
	$b = array(1.5, -22.5, 3.5, 127.5, -300.5, 2.5);
	echo in_array($a, -300), "\n";
	echo in_array($a, 301), "\n";
	echo in_array($b, 2.5), "\n";
	echo in_array($b, 4.5), "\n";
	echo in_array($a, 2.7), "\n";	// cast to int
	echo in_array($a, 127.1), "\n";	// cast to int
}

test_function();

?>
--EXPECT--
1
0
1
0
1
1
