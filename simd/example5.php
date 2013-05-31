<?php

/**
 * @engine qb
 * @local int32[4] $a
 * @local int32[4] $b
 * @local int32[4] $c
 */
function test() {
	$a = array(4, 3, 2, 1);
	$b = array(1, 2, 3, 4);
	$c = $a + $b;
	echo "$c\n";
}

qb_compile();
test();

?>