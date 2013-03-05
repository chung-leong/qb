--TEST--
Argument wrap-around test (string input)
--FILE--
<?php

/**
 * @engine qb
 * @param uint32[10]	$a
 */
function test_function($a) {
	echo "$a\n";
}

qb_compile();

test_function("\x08\x00\x00\x00\x09\x00\x00\x00\x0A\x00\x00\x00");

?>
--EXPECT--
[8, 9, 10, 8, 9, 10, 8, 9, 10, 8]
