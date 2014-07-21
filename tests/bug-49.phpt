--TEST--
Bug #49
--FILE--
<?php

/**
 * @engine qb
 * @static float32[2]	$m1
 * @static float32[2]	$m2
 */
function test() {
	static $m1 = array(1, 2);
	static $m2 = array(3, 4);
	echo "$m1 $m2\n";
	$m1 *= 2;
	$m2 *= 2;
}

test();
test();

?>
--EXPECT--
[1, 2] [3, 4]
[2, 4] [6, 8]

