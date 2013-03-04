--TEST--
Constant as array length test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[LENGTH]		$a
 * 
 * @return	void
 * 
 */
function test_function() {
	echo "$a\n";
}

define('LENGTH', 4);

qb_compile();

test_function();

?>
--EXPECT--
[0, 0, 0, 0]
