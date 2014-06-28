--TEST--
Bug #42
--FILE--
<?php

/**
 * @engine qb
 * @param float32[]		$list1
 * @param float32[][2]	$list2
 * @local float32		$x
 * @local float32[2]	$y
 */
function test($list1, $list2) {
	$x = 8;
	$y = array(1, 2);
	$list1[] = $x / 2;
	$list2[] = ($y * 2) + 3;
	echo "{$list1} {$list2}\n";
}

test(array(1, 2, 3), array());

?>
--EXPECT--
[1, 2, 3, 4] [[5, 7]]

