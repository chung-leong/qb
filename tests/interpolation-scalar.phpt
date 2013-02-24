--TEST--
Variable interpolation test (scalar)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int16	$a
 * @local	int8	$b
 * @local	uint32	$c
 * @local	int32	$d
 * @local	float32	$e
 * @local	float64	$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1234;
	$b = -5;
	$c = 3000;
	$d = 0xFFFFFFFF;
	$e = 0.25;
	$f = 5000000.5;
	echo "Numbers: $a, $b, $c, $d, $e, $f";
}

qb_compile();

test_function();

?>
--EXPECT--
Numbers: 1234, -5, 3000, -1, 0.25, 5000000.5
