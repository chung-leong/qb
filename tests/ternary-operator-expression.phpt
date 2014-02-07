--TEST--
Ternary operator test (more complex expression)
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
	$b = ($a > 10) ? $a + 123 : $a + 456;
	echo "$b\n";
}

test_function(12);
test_function(5);

?>
--EXPECT--
135
461
