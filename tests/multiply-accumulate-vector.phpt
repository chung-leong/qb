--TEST--
Multiply accumulate test (vector)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]	$[a-z]
 * @local	float64[4]	$[A-Z]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4);
	$b = array(5, 6, 7, 8);
	$c = $a + ($a * $b);
	echo "$c\n";
	$A = array(1, 2, 3, 4);
	$B = array(5, 6, 7, 8);
	$C = $A + ($A * $B);
	echo "$C\n";
}

test_function();

?>
--EXPECT--
[6, 14, 24, 36]
[6, 14, 24, 36]
