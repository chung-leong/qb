--TEST--
Array pad test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]		$a
 * @local	int32			$b
 * @local	float32[*][2]	$c
 * @local	int32[2]		$d;
 *
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3);
	$b = 42;
	echo array_pad($a, 8, $b), "\n";
	echo array_pad(array_pad($a, 8, $b), -16, 43), "\n";
	
	$d = array(1, 2);
	echo array_pad($c, 3, array(3.3, 4.4)), "\n";
	echo array_pad($c, -3, array(3.3, 4.4)), "\n";
	echo array_pad($c, -4, $d), "\n";
}

test_function();

?>
--EXPECT--
[1, 2, 3, 42, 42, 42, 42, 42]
[43, 43, 43, 43, 43, 43, 43, 43, 1, 2, 3, 42, 42, 42, 42, 42]
[[3.3, 4.4], [3.3, 4.4], [3.3, 4.4]]
[[3.3, 4.4], [3.3, 4.4], [3.3, 4.4]]
[[1, 2], [1, 2], [1, 2], [1, 2]]
