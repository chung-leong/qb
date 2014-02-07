--TEST--
Argument by value test (byte string, U32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint32[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('I*', 4294967295, -1, 0);

test_function($data);

?>
--EXPECT--
Numbers: [4294967295, 4294967295, 0]