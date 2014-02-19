--TEST--
Flush test
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
	echo "Hello world\n";
	flush();
}

test_function();

?>
--EXPECT--
Hello world