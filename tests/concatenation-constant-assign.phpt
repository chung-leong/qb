--TEST--
String concatenation assignment test (constant)
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
	$a = "pi = ";
	$a .= 3.14;
	$a .= 159;
	echo $a;
}

test_function();

?>
--EXPECT--
pi = 3.14159
