<?php

/**
 * @engine qb
 * @local int32[4] $b
 * @local int32[4] $c
 */
function test() {
	$b = array(1, 2, 3, 4);
	$c = $b + 5;
	echo "$c\n";
}

qb_compile();
test();

?>