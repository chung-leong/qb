--TEST--
Return variable test (array element)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[4]	$.*
 * 
 * @return	int32
 * 
 */
function test_function() {
	$a = array(1234, 5678, 9, 10);
	return $a[1];
}

echo test_function();

?>
--EXPECT--
5678
