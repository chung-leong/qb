--TEST--
Ternary operator test (nested)
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
	$b = ($a > 10) ? (($a % 2 == 0) ? 122 : 123) : (($a % 3 == 0) ? 455 : 456);
	echo "$b\n";
}

qb_compile();

test_function(12);
test_function(13);
test_function(6);
test_function(5);

?>
--EXPECT--
122
123
455
456
