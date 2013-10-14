--TEST--
String concatenation test
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
	echo $a . " " . $b;
}

test_function();

?>
--EXPECT--
hello world
