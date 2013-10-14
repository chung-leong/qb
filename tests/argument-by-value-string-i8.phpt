--TEST--
Argument by value test (byte string, I08)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int8[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('c*', 123, 124, 77, 54, 72);

test_function($data);

?>
--EXPECT--
Numbers: [123, 124, 77, 54, 72]