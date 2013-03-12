--TEST--
String concatenation test (named constant)
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
	$a = "pi = " . M_PI . " (type = " . float64 . ")\n";
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
pi = 3.1415926535898 (type = 9)
