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
	$result = pack_be($a);
	echo "$result\n";
	$result = pack_be($b);
	echo "$result\n";
	$result = pack_be(100000, int32);
	echo "$result\n";
	$result = pack_be(0xFFAA0011, uint32);
	echo "$result\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0, 0, 0, 1]
[0, 0, 3, 232]
[0, 1, 134, 160]
[255, 170, 0, 17]