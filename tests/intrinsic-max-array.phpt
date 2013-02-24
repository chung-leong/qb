--TEST--
Maximum number test (array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, -22, 3, 127, -300000, 2);
	echo max($a), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
127
