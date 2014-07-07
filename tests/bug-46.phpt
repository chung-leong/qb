--TEST--
Bug #46
--FILE--
<?php

/**
 * @engine	qb 
 * @global	int32[][4]	$a
 */
function test() {
	global $a;
	echo count($a);
}

$a = array();

test();

?>
--EXPECT--
0
