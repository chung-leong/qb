--TEST--
PHP version test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	echo phpversion(), "\n";
	echo phpversion('qb'), "\n";
}

test_function();

?>
--EXPECTREGEX--
5\.\d\.\d+
2\.\d+
