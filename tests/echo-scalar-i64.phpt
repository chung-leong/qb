--TEST--
Echo variable test (scalar, i64)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int64	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = -3;
	echo $a;
}

test_function();

?>
--EXPECT--
-3
