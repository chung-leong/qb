--TEST--
Variable interpolation test (string)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "hello";
	$b = "world";
	echo "Strings: $a, $b";
}

test_function();

?>
--EXPECT--
Strings: hello, world
