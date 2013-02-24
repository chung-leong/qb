--TEST--
Argument by value test (byte string, U16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint16[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

qb_compile();

$data = pack('S*', 65535, 65534, -1, 0, 1);

test_function($data);

?>
--EXPECT--
Numbers: [65535, 65534, 65535, 0, 1]