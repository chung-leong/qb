<?php

/**
 * @engine qb
 * @local float32[4] $b
 * @local float32[4] $c
 */
function test() {
	$b = array(0.1, 0.2, 0.3, 0.4);
	$c = cos($b);
	echo "$c\n";
}

qb_compile();
test();

?>