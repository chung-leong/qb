--TEST--
Unpack test (big-endian, i32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int8[]	$data
 * @local	int32	$[ab]
 * 
 * @return	void
 * 
 */
function test_function() {
	$data = "\x01\x02\x03\x04\x05\x06\x07\x08";
	$a = unpack_be($data);
	$b = unpack_be($data, 1);
	echo "$a\n";
	echo "$b\n";
	echo unpack_be($data, 2, int32), "\n";
	echo unpack_be($data, 1, int16), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
16909060
33752069
50595078
515
