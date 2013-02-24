--TEST--
Return variable test (array, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[4]	$.*
 * 
 * @return	int32[4]
 * 
 */
function test_function() {
	$a = array(1234, 5678, 9, 10);
	return $a;
}

qb_compile();

echo implode(test_function(), ', ');

?>
--EXPECT--
1234, 5678, 9, 10
