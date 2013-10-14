--TEST--
Argument by value test (byte string, U08)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint8[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('C*', 255, 234, 77, 54, 200, 201, 0, 0, 7);

test_function($data);

?>
--EXPECT--
Numbers: [255, 234, 77, 54, 200, 201, 0, 0, 7]