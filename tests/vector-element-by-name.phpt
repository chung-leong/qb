--TEST--
Vector element by name test
--FILE--
<?php

/**
 * @engine qb
 * @local vector2 $a
 */
function test_function() {
	$a->x = 3;
	$a->y = 2;
	echo "$a\n";
	echo "$a->x, $a->y\n";
}

test_function();

?>
--EXPECT--
[3, 2]
3, 2
