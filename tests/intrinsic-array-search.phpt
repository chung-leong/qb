--TEST--
Array search test
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
	echo array_search($a, -300), "\n";
	echo array_search($a, 301), "\n";
	echo array_search($b, 2.5), "\n";
	echo array_search($b, 4.5), "\n";
	echo array_search($a, 2.7), "\n";	// cast to int
	echo array_search($a, 127.1), "\n";	// cast to int
}

test_function();

?>
--EXPECT--
4
-1
5
-1
5
3
