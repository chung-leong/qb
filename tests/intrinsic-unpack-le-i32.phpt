--TEST--
Unpack test (little-endian, i32)
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
	$a = unpack_le($data);
	$b = unpack_le($data, 1);
	echo "$a\n";
	echo "$b\n";
	echo unpack_le($data, 2, int32), "\n";
	echo unpack_le($data, 1, int16), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
67305985
84148994
100992003
770
