--TEST--
String concatenation assignment test (I32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$b
 * @local	string	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "hello";
	$b = -2001;
	$a .= $b;
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
hello-2001
