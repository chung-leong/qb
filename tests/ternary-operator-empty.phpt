--TEST--
Ternary operator test (empty true clause)
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
	$b = $a ? : 456;
	echo "$b\n";
}

qb_compile();

test_function(123);
test_function(0);

?>
--EXPECT--
123
456
