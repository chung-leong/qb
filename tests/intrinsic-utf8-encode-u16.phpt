--TEST--
UTF8 encode test (U16)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint16[]	$cp
 * 
 * @return	void
 * 
 */
function test_function() {
	$cp = array(67, 122, 101, 347, 263, 44, 32, 106, 97, 107, 32, 115, 105, 281, 32, 109, 97, 115, 122, 63);
	echo utf8_encode($cp);
}

test_function();

?>
--EXPECT--
Cześć, jak się masz?
