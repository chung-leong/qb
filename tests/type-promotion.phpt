--TEST--
Type promotion test (scalar, f32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$x
 * @local	uint32	$w
 * @local	float32	$v
 * 
 * @return	void
 * 
 */
function test_function() {
	$w = 500;
	$x = 200;
	$v = 2.0 * $x / $w + 1.0;
	echo "$v\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1.8
