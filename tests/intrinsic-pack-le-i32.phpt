--TEST--
Pack test (little-endian, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint8[4]	$result
 * @local	int32		$[ab]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1;
	$b = 1000;
	$result = pack_le($a);
	echo "$result\n";
	$result = pack_le($b);
	echo "$result\n";
	$result = pack_le(100000, int32);
	echo "$result\n";
	$result = pack_le(0xFFAA0011, uint32);
	echo "$result\n";
}

test_function();

?>
--EXPECT--
[1, 0, 0, 0]
[232, 3, 0, 0]
[160, 134, 1, 0]
[17, 0, 170, 255]