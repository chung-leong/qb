--TEST--
Argument by value test (byte string, U64)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint64[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

qb_compile();

$data = pack('I*', 4294967295, 4294967295, 4294967295, 0, 1, 0, 0, 1);

test_function($data);

// we'd only get the expected result on platforms with little endian byte order

?>
--EXPECT--
Numbers: [18446744073709551615, 4294967295, 1, 4294967296]