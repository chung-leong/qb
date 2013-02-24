--TEST--
Return variable test (array as scalar)
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
	return $a;
}

qb_compile();

echo test_function();

?>
--EXPECTREGEX--
Fatal error.*
