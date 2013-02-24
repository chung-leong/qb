--TEST--
Array sum test
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
	$a = array(1, -22, 3, 127, -300, 2);
	echo array_sum($a), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
-189
