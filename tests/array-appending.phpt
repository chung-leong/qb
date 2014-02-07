--TEST--
Array appending test
--FILE--
<?php

/**
 * @engine qb
 * @local int32[]	$a
 * @local int32[][4]	$b
 * @return void
 */
function test_function() {
	$a[] = 5;
	$a[] = 6;
	$b[] = array(1, 2, 3, 4);
	$b[] = array(5, 6, 7, 8);
	echo "$a $b\n";
}

test_function();

?>
--EXPECT--
[5, 6] [[1, 2, 3, 4], [5, 6, 7, 8]]
