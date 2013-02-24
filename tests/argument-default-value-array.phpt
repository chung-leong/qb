--TEST--
Default argument test (array)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[]		$a
 * @param	int32[]		$b
 */
function test_function($a, $b = array(5, 6, 7, 8)) {
	echo "$a $b\n";
}

qb_compile();

$a = array(1, 2, 3, 4);
test_function($a);

$a[] = 5;
test_function($a, $a);


?>
--EXPECT--
[1, 2, 3, 4] [5, 6, 7, 8]
[1, 2, 3, 4, 5] [1, 2, 3, 4, 5]
