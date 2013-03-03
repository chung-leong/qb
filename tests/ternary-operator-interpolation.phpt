--TEST--
Ternary operator test (string interpolation)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64	$a
 * @local	string	$b
 * 
 * @return	void
 * 
 */
function test_function($a) {
	$b = ($a > 10) ? "sin($a) = " . sin($a) : "cos($a) = " . cos($a);
	echo "$b\n";
}

qb_compile();

test_function(M_PI * 10.5);
test_function(M_PI * 1);

?>
--EXPECT--
sin(32.986722862693) = 1
cos(3.1415926535898) = -1
