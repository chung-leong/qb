--TEST--
Zend version test
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
	echo zend_version(), "\n";
}

test_function();

?>
--EXPECTREGEX--
2\.\d\.\d+
