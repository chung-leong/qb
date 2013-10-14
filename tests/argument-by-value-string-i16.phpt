--TEST--
Argument by value test (byte string, I16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int16[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('s*', 10255, 10234, -1077, 1054, -10200, 201, 0, 0, 7);

test_function($data);

?>
--EXPECT--
Numbers: [10255, 10234, -1077, 1054, -10200, 201, 0, 0, 7]