--TEST--
Bug #42
--FILE--
<?php

/**
 * @engine qb
 * @param float32[]	$list
 * @local float32	$x
 */
function test($list) {
	$x = 8;
	$list[] = $x;
	echo "{$list}\n";
}

test(array(1, 2, 3));

?>
--EXPECT--
[4]

