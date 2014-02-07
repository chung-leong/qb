--TEST--
Default argument test (scalar)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[]		$a
 * @param	int32		$b
 */
function test_function($a, $b = 1234) {
	echo "$a $b";
}

$a = array(1, 2, 3, 4);
test_function($a);

?>
--EXPECT--
[1, 2, 3, 4] 1234
