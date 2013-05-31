<?php

/**
 * @engine qb
 * @local float32 $a
 * @local float32[4] $b
 * @local float32[4] $c
 */
function test() {
	$a = 2;
	$b = array(1, 2, 3, 4);
	$c = pow($b, $a);          /* $a is the exponent here */
	echo "$c\n";
}

qb_compile();
test();

?>