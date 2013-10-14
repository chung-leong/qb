--TEST--
Array product test
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
	echo array_product($a), "\n";
}

test_function();

?>
--EXPECT--
5029200
