--TEST--
Argument by value test (byte string, I64)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int64[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('I*', 4294967295, 4294967295, 4294967295, 0, 1, 0, 0, 1);

test_function($data);

// we'd only get the expected result on platforms with little endian byte order

?>
--EXPECT--
Numbers: [-1, 4294967295, 1, 4294967296]