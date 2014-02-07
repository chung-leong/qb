--TEST--
Unsupported language feature test
--FILE--
<?php

/**
 * @engine qb
 * @param uint32[12]	$a
 */
function test_function($a) {
	echo "Hello world";
	require("test.php");
	echo "$a\n";
}

test_function(array(1, 2, 3, 4));

?>
--EXPECTREGEX--
.*Fatal error.*line 9
