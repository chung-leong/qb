--TEST--
Default argument test (string)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[]		$a
 * @param	string		$b
 */
function test_function($a, $b = "Hello world") {
	echo "$a $b\n";
}

$a = array(1, 2, 3, 4);
test_function($a);

test_function($a, "IDIC");


?>
--EXPECT--
[1, 2, 3, 4] Hello world
[1, 2, 3, 4] IDIC
