--TEST--
Return variable test (array size mismatch)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[4]	$.*
 * 
 * @return	int32[6]
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
1234, 5678, 9, 10, 1234, 5678
