<?php

/**
 * @engine qb
 * @local int32[4] $b
 */
function test() {
	$b = rand(1, 100);
	echo "$b\n";
}

qb_compile();
test();

?>