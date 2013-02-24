--TEST--
Print test (scalar)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1234;
	($a < 0) or print "Hello world $a";
}

qb_compile();

test_function();

?>
--EXPECT--
Hello world 1234