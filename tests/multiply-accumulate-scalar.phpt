--TEST--
Multiply accumulate test (scalar, f32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$[a-z]
 * @local	float64	$[A-Z]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 10.5;
	$b = 5.5;
	$c = $a + ($a * $b);
	echo "$c\n";
	$A = 10.5;
	$B = 5.5;
	$C = $A + ($A * $B);
	echo "$Cc\n";
}

qb_compile();

test_function();

?>
--EXPECT--
68.25
68.25
