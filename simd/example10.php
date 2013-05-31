<?php

/**
 * @engine qb
 * @local int32[2] $a
 * @local int32[3] $b
 * @local int32[3] $c
 */
function test() {
	$a = array(5, 6);
	$b = array(10, 20, 30);
	$c = $a + $b;
	echo "$c\n";
}

qb_compile();
test();

?>