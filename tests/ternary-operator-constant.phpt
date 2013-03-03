--TEST--
Ternary operator test (constants)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$a
 * @local	int32	$b
 * 
 * @return	void
 * 
 */
function test_function($a) {
	$b = ($a > 10) ? 123 : 456;
	echo "$b\n";
}

qb_compile();

test_function(12);
test_function(5);

?>
--EXPECT--
123
456
