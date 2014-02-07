--TEST--
Argument wrap-around test
--FILE--
<?php

/**
 * @engine qb
 * @param uint32[12]	$a
 */
function test_function($a) {
	echo "$a\n";
}

test_function(array(1, 2, 3, 4));

?>
--EXPECT--
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4]
