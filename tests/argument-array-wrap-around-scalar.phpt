--TEST--
Argument wrap-around test (scalar input)
--FILE--
<?php

/**
 * @engine qb
 * @param uint32[12]	$a
 */
function test_function($a) {
	echo "$a\n";
}

test_function(9);

?>
--EXPECT--
[9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9]
