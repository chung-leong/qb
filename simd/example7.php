<?php

/**
 * @engine qb
 * @local float32[4] $a
 * @local float32[4] $b
 * @local float32[4] $c
 */
function test() {
	$a = 10;
	$b = array(1, 2, 3, 4);
	$c = pow($a, $b);
	echo "$c\n";
}

qb_compile();
test();

?>