--TEST--
UTF8 decode test (U16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	string		$s
 * @local	uint16[]	$cp
 * 
 * @return	void
 * 
 */
function test_function($s) {
	$cp = utf8_decode($s);
	echo $cp;
}

qb_compile();

test_function("Cześć, jak się masz?");

?>
--EXPECT--
[67, 122, 101, 347, 263, 44, 32, 106, 97, 107, 32, 115, 105, 281, 32, 109, 97, 115, 122, 63]
